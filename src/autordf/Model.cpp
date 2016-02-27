#include "autordf/Model.h"

#include <errno.h>
#include <string.h>
#include <librdf.h>

#include <sstream>
#include <set>

#include "autordf/internal/World.h"
#include "autordf/internal/ModelPrivate.h"
#include "autordf/internal/Storage.h"
#include "autordf/internal/Parser.h"
#include "autordf/internal/Stream.h"
#include "autordf/internal/Uri.h"
#include "autordf/internal/StatementConverter.h"
#include "autordf/Exception.h"

namespace autordf {

using namespace internal;

Model::Model() : _world(new World()), _model(new ModelPrivate(std::make_shared<Storage>())) {
}

void Model::loadFromFile(const std::string& path, const std::string& baseIRI) {
    FILE *f = ::fopen(path.c_str(), "r");
    if ( !f ) {
        std::stringstream ss;
        ss << "Unable to open " << path << ": " << ::strerror(errno);
        throw FileIOError(ss.str().c_str());
    }

    try {
        loadFromFile(f, path, baseIRI);
    }
    catch(...) {
        ::fclose(f);
        throw;
    }
    ::fclose(f);
}

void Model::loadFromFile(FILE *fileHandle, const std::string& format, const std::string& baseIRI) {
    std::shared_ptr<Parser> p = Parser::guessFromExtension(format);
    if ( !p ) {
        throw UnsupportedRdfFileFormat("File format not recognized");
    }
    if ( librdf_parser_parse_file_handle_into_model(p->get(), fileHandle, 0, (baseIRI.length() ? Uri(baseIRI).get() : nullptr), _model->get()) ) {
        throw InternalError("Failed to read model from file");
    }
    int prefixCount = librdf_parser_get_namespaces_seen_count(p->get());
    for ( unsigned int i = 0; i < prefixCount; ++i ) {
        const char * prefix = librdf_parser_get_namespaces_seen_prefix(p->get(), i);
        librdf_uri * uri = librdf_parser_get_namespaces_seen_uri(p->get(), i);
        if ( prefix && uri ) {
            addNamespacePrefix(prefix, reinterpret_cast<char*>(librdf_uri_as_string(uri)));
        }
    }
}

void Model::saveToFile(const std::string& path, const std::string& baseIRI, const char *format) {
    if ( !format ) {
        format = librdf_parser_guess_name2(_world->get(), NULL, NULL, reinterpret_cast<const unsigned char *>(path.c_str()));
    }
    if ( !format ) {
        throw UnsupportedRdfFileFormat("Unable to deduce format from file save name");
    }

    FILE *f = ::fopen(path.c_str(), "w");
    if ( !f ) {
        std::stringstream ss;
        ss << "Unable to open " << path << ": " << ::strerror(errno);
        throw FileIOError(ss.str().c_str());
    }

    try {
        saveToFile(f, format, baseIRI);
    }
    catch(...) {
        ::fclose(f);
        throw;
    }
    ::fclose(f);
}

void Model::saveToFile(FILE *fileHandle, const char *format, const std::string& baseIRI) {
    std::shared_ptr<librdf_serializer> s(librdf_new_serializer(_world->get(), format, 0, 0), librdf_free_serializer);
    if ( !s ) {
        throw InternalError("Failed to construct RDF serializer");
    }

    for ( auto const& pfx : _namespacesPrefixes ) {
        int ret = librdf_serializer_set_namespace(s.get(), Uri(pfx.second + "#").get(), pfx.first.c_str() );
        // 0 is Ok
        // 1 means namespace has already been registered, which can happen if we read a file with namespace and safe it afterwards: we ignore that one
        if ( ret > 1 ) {
            throw InternalError(std::string("Failed to set namespace prefix map for RDF serializer: ") + pfx.first.c_str() + "-->" + pfx.second);
        }
    }

    if ( librdf_serializer_serialize_model_to_file_handle(s.get(), fileHandle, baseIRI.length() ? Uri(baseIRI).get() : nullptr, _model->get()) ) {
        throw InternalError("Failed to export RDF model to file");
    }
}

StatementList Model::find(const Statement& req) const {
    return StatementList(req, this);
}

/**
 * Return the sources (subjects) of arc in an RDF graph given arc (predicate) and target (object).
 */
NodeList Model::findSources(const Node& arc, const Node& target) const {
    return NodeList(Node(), arc, target, this);
}

/**
 * Return the arcs (predicates) of an arc in an RDF graph given source (subject) and target (object).
 */
NodeList Model::findArcs(const Node& source, const Node& target) const {
    return NodeList(source, Node(), target, this);
}

/**
 * Return the targets (objects) of an arc in an RDF graph given source (subject) and arc (predicate).
 */
NodeList Model::findTargets(const Node& source, const Node& arc) const {
    return NodeList(source, arc, Node(), this);
}

/**
     * Return one arc (predicate) of an arc in an RDF graph given source (subject) and arc (predicate).
 */
Node Model::findTarget(const Node& source, const Node& arc) const {
    return Node(librdf_model_get_target(_model->get(), source.get(), arc.get()), true);
}

void Model::add(const Statement &stmt) {
    std::shared_ptr<librdf_statement> librdfstmt(StatementConverter::toLibRdfStatement(stmt));
    if ( librdf_model_add_statement (_model->get(), librdfstmt.get()) ) {
        std::stringstream ss;
        ss << "Unable to add '" << stmt << "' statement";
        throw InternalError(ss.str());
    }
}

void Model::remove(const Statement &stmt) {
    std::shared_ptr<librdf_statement> librdfstmt(StatementConverter::toLibRdfStatement(stmt));
    if ( librdf_model_remove_statement (_model->get(), librdfstmt.get()) ) {
        std::stringstream ss;
        ss << "Unable to remove '" << stmt << "' statement";
        throw InternalError(ss.str());
    }
}

const std::string& Model::nsToPrefix(const std::string& ns) const {
    std::string cleanedNs = cleanNamespaceTrailingChars(ns);
    for ( auto const& p : _namespacesPrefixes) {
        if ( p.second == cleanedNs ) {
            return p.first;
        }
    }
    throw std::out_of_range("Namespace " + ns + " not found in Model namespace map");
}

/**
 * Returns the prefix that matches the given rdfiri if a prefix is registered, empty otherwise
 */
std::string Model::iriPrefix(const std::string& rdfiri) const {
    for ( const std::pair<std::string, std::string>& prefixMapItem : _namespacesPrefixes ) {
        const std::string& iri = prefixMapItem.second;
        if ( rdfiri.substr(0, iri.length()) == iri ) {
            return prefixMapItem.first;
        }
    }
    return "";
}


const std::string& Model::prefixToNs(const std::string& prefix) const {
    return _namespacesPrefixes.at(prefix);
}

void Model::addNamespacePrefix(const std::string& prefix, const std::string& ns) {
    std::string cleanedNs = cleanNamespaceTrailingChars(ns);
    auto it = _namespacesPrefixes.find(prefix);
    if ( it == _namespacesPrefixes.end() ) {
        _namespacesPrefixes[prefix] = cleanedNs;
    } else if ( it->second != cleanedNs ) {
        throw InternalError("Unable to add prefix " + prefix + "-->" + cleanedNs + " mappping: already registered to " + it->second);
    }
}

std::string Model::cleanNamespaceTrailingChars(std::string ns) {
    static const std::set<char> IGNORED_LAST_CHARS = {'#', ':', '/'};
    if ( IGNORED_LAST_CHARS.count(ns.back()) ) {
        ns.pop_back();
    }
    return ns;
}

}