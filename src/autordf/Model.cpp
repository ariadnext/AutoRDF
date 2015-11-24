#include "autordf/Model.h"

#include <errno.h>
#include <string.h>
#include <librdf.h>

#include <sstream>

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

void Model::loadFromFile(const std::string& path, const std::string& baseUri) {
    FILE *f = ::fopen(path.c_str(), "r");
    if ( !f ) {
        std::stringstream ss;
        ss << "Unable to open " << path << ": " << ::strerror(errno);
        throw FileIOError(ss.str().c_str());
    }

    try {
        std::shared_ptr<Parser> p = Parser::guessFromUri(Uri(path));
        if ( !p ) {
            throw UnsupportedRdfFileFormat("File format not recognized");
        }
        if ( librdf_parser_parse_file_handle_into_model(p->get(), f, 0, Uri(baseUri).get(), _model->get()) ) {
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
    catch(...) {
        ::fclose(f);
        throw;
    }
    ::fclose(f);
}

void Model::saveToFile(const std::string& path, const std::string& baseUri, const char *format) {
    if ( !format ) {
        format = librdf_parser_guess_name2(_world->get(), NULL, NULL, reinterpret_cast<const unsigned char *>(path.c_str()));
    }
    if ( !format ) {
        throw UnsupportedRdfFileFormat("Unable to deduce format from file save name");
    }

    std::shared_ptr<librdf_serializer> s(librdf_new_serializer(_world->get(), format, 0, 0), librdf_free_serializer);
    if ( !s ) {
        throw InternalError("Failed to construct RDF serializer");
    }

    if ( librdf_serializer_serialize_model_to_file(s.get(), path.c_str(), baseUri.length() ? Uri(baseUri).get() : nullptr, _model->get()) ) {
        throw InternalError("Failed to export RDF model to file");
    }
}

StatementList Model::find(const Statement& req) {
    return StatementList(req, this);
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
    for ( auto const& p : _namespacesPrefixes) {
        if ( p.second == ns ) {
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
    auto it = _namespacesPrefixes.find(prefix);
    if ( it == _namespacesPrefixes.end() ) {
        _namespacesPrefixes[prefix] = ns;
    } else if ( it->second != ns ) {
        throw InternalError("Unable to add prefix " + prefix + "-->" + ns + " mappping: already registered to " + it->second);
    }
}
}