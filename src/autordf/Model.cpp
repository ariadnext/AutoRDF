#include "autordf/internal/cAPI.h"
#include "autordf/Model.h"

#include <errno.h>
#include <string.h>

#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>

#include "autordf/internal/World.h"
#include "autordf/internal/ModelPrivate.h"
#include "autordf/internal/Stream.h"
#include "autordf/internal/StatementConverter.h"
#include "autordf/Exception.h"
#ifdef USE_REDLAND
#include "autordf/internal/Parser.h"
#include "autordf/internal/Uri.h"
#endif

namespace autordf {

using namespace internal;

#if defined(USE_REDLAND)

void retrieveSeenNamespaces(std::shared_ptr<internal::Parser> parser, Model *model, std::string* baseIRI) {
    int prefixCount = librdf_parser_get_namespaces_seen_count(parser->get());
    for ( unsigned int i = 0; i < prefixCount; ++i ) {
        const char * prefix = librdf_parser_get_namespaces_seen_prefix(parser->get(), i);
        librdf_uri * uri = librdf_parser_get_namespaces_seen_uri(parser->get(), i);
        if ( prefix && uri ) {
            model->addNamespacePrefix(prefix, reinterpret_cast<char*>(librdf_uri_as_string(uri)));
        } else if ( !prefix && uri ) {
            *baseIRI = reinterpret_cast<char*>(librdf_uri_as_string(uri));
        }
    }
}

std::shared_ptr<librdf_serializer> prepareSerializer(const char *format, const std::map<std::string, std::string>& namespacesPrefixes) {
    std::shared_ptr<librdf_serializer> s(librdf_new_serializer(World().get(), format, 0, 0), librdf_free_serializer);
    if ( !s ) {
        throw InternalError("Failed to construct RDF serializer");
    }

    for ( auto const& pfx : namespacesPrefixes ) {
        int ret = librdf_serializer_set_namespace(s.get(), Uri(pfx.second).get(), pfx.first.c_str() );
        // 0 is Ok
        // 1 means namespace has already been registered, which can happen if we read a file with namespace and safe it afterwards: we ignore that one
        if ( ret > 1 ) {
            throw InternalError(std::string("Failed to set namespace prefix map for RDF serializer: ") + pfx.first.c_str() + "-->" + pfx.second);
        }
    }
    return s;
}

Model::Model() : _world(new World()), _model(new ModelPrivate(std::make_shared<Storage>())) {
}

Model::Model(std::shared_ptr<Storage> storage) : _world(new World()), _model(new ModelPrivate(storage)) {
}

void Model::loadFromFile(const std::string& path, const std::string& baseIRI) {
    FILE *f = ::fopen(path.c_str(), "r");
    if ( !f ) {
        std::stringstream ss;
        ss << "Unable to open " << path << ": " << ::strerror(errno);
        throw FileIOError(ss.str().c_str());
    }

    const char *format = librdf_parser_guess_name2(_world->get(), NULL, NULL, reinterpret_cast<const unsigned char *>(path.c_str()));
    if ( !format ) {
        throw UnsupportedRdfFileFormat("Unable to deduce format from file save name");
    }
    try {
        loadFromFile(f, format, baseIRI, path);
    }
    catch(...) {
        ::fclose(f);
        throw;
    }
    ::fclose(f);
}

void Model::loadFromMemory(const void* data, const char *format, const std::string& baseIRI) {
    std::shared_ptr<Parser> p = std::make_shared<Parser>(format);
    if ( !p ) {
        throw UnsupportedRdfFileFormat("File format not recognized");
    }
    if ( librdf_parser_parse_string_into_model(p->get(), static_cast<const unsigned char *>(data), (baseIRI.length() ? Uri(baseIRI).get() : Uri(".").get()), _model->get()) ) {
        throw InternalError("Failed to read model from stream");
    }
    _baseUri = baseIRI;
    retrieveSeenNamespaces(p, this, &_baseUri);
}

void Model::loadFromFile(FILE *fileHandle, const char *format, const std::string& baseIRI, const std::string& streamInfo) {
    std::shared_ptr<Parser> p = std::make_shared<Parser>(format);
    if ( !p ) {
        throw UnsupportedRdfFileFormat(streamInfo + ": File format not recognized");
    }
    if ( librdf_parser_parse_file_handle_into_model(p->get(), fileHandle, 0, (baseIRI.length() ? Uri(baseIRI).get() : Uri(".").get()), _model->get()) ) {
        throw InternalError(streamInfo + ": Failed to read model from stream");
    }
    _baseUri = baseIRI;
    retrieveSeenNamespaces(p, this, &_baseUri);
}

void Model::saveToFile(const std::string& path, const std::string& baseIRI, bool enforceRepeatable, const char *format) {
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
        saveToFile(f, format, baseIRI, enforceRepeatable);
    }
    catch(...) {
        ::fclose(f);
        throw;
    }
    ::fclose(f);
}

class StatementVector : public std::vector<std::shared_ptr<librdf_statement> > {
public:
    void sort() {
        return std::sort(begin(), end(),
                         [](const std::shared_ptr<librdf_statement>& l, const std::shared_ptr<librdf_statement>& r) { return encodeStatement(l) < encodeStatement(r);}
        );
    }
private:
    static std::string encodeStatement(const std::shared_ptr<librdf_statement>& stmt) {
        size_t sz = librdf_statement_encode_parts2(World().get(), stmt.get(), nullptr, nullptr, 0, LIBRDF_STATEMENT_ALL);
        std::string encoded(sz, '\0');
        librdf_statement_encode_parts2(World().get(), stmt.get(), nullptr, reinterpret_cast<unsigned char*>(const_cast<char*>(encoded.data())), sz, LIBRDF_STATEMENT_ALL);
        return encoded;
    }
};

extern "C" int stream_is_end_method(void *param);
extern "C" int stream_next_method(void *param);
extern "C" void *stream_get_method(void *param, int value);

class StatementsStream {
public:
    StatementsStream(StatementVector* v) {
        _statements = v;
        _iter = v->begin();
    }

    std::shared_ptr<librdf_stream> newRdfStream() {
        return std::shared_ptr<librdf_stream>(librdf_new_stream(World().get(), this,
                                                                &autordf::stream_is_end_method,
                                                                &autordf::stream_next_method,
                                                                &autordf::stream_get_method,
                                                                nullptr),
                                              librdf_free_stream);
    }

    // Implementation details
    int is_end_method() {
        return _statements->end() == _iter;
    }

    int stream_next_method() {
        ++_iter;
        return _statements->end() == _iter;
    }

    void *stream_get_method(int) {
        return _iter->get();
    }

private:
    StatementVector* _statements;
    StatementVector::iterator _iter;
};

extern "C" int stream_is_end_method(void *param) {
    return static_cast<StatementsStream*>(param)->is_end_method();
}

extern "C" int stream_next_method(void *param) {
    return static_cast<StatementsStream*>(param)->stream_next_method();
}

extern "C" void *stream_get_method(void *param, int value) {
    return static_cast<StatementsStream*>(param)->stream_get_method(value);
}

void Model::saveToFile(FILE *fileHandle, const char *format, const std::string& baseIRI, bool enforceRepeatable) {
    std::shared_ptr<librdf_serializer> s = prepareSerializer(format, _namespacesPrefixes);

    std::shared_ptr<librdf_stream> sourcestream(librdf_model_as_stream(_model->get()), librdf_free_stream);

    if ( enforceRepeatable ) {
        StatementVector statements;
        int statementsCount = librdf_model_size(_model->get());
        if ( statementsCount > 0 ) {
            statements.reserve(statementsCount);
        }
        while (!librdf_stream_end(sourcestream.get())) {
            librdf_statement *statement = librdf_stream_get_object(sourcestream.get());
            statements.emplace_back(std::shared_ptr<librdf_statement>(librdf_new_statement_from_statement(statement), librdf_free_statement));
            librdf_stream_next(sourcestream.get());
        }
        statements.sort();

        StatementsStream ss(&statements);
        if ( librdf_serializer_serialize_stream_to_file_handle(s.get(), fileHandle, baseIRI.length() ? Uri(baseIRI).get() : nullptr, ss.newRdfStream().get()) ) {
            throw InternalError("Failed to export RDF model to file");
        }
    } else {
        if ( librdf_serializer_serialize_stream_to_file_handle(s.get(), fileHandle, baseIRI.length() ? Uri(baseIRI).get() : nullptr, sourcestream.get()) ) {
            throw InternalError("Failed to export RDF model to file");
        }
    }
}

std::shared_ptr<std::string> Model::saveToMemory(const char *format, const std::string& baseIRI) {
    std::shared_ptr<librdf_serializer> s = prepareSerializer(format, _namespacesPrefixes);
    unsigned char * serialized = librdf_serializer_serialize_model_to_string(s.get(), baseIRI.length() ? Uri(baseIRI).get() : nullptr, _model->get());
    if ( serialized ) {
        auto res = std::make_shared<std::string>(reinterpret_cast<char*>(serialized));
        librdf_free_memory(serialized);
        return res;
    } else {
        throw InternalError("Failed to export RDF model to memory");
    }
}

std::list<std::string> Model::supportedFormat() const {
    std::list<std::string> format;
    unsigned int i = 0;
    const raptor_syntax_description *descr = librdf_serializer_get_description(_world->get(), i++);

    while(descr != NULL) {
        for (unsigned int j = 0; j < descr->names_count; j++) {
            format.push_back(descr->names[j]);
        }
        descr = librdf_serializer_get_description(_world->get(), i++);
    };
    return format;
}

void Model::add(Statement *stmt) {
    std::shared_ptr<librdf_statement> librdfstmt(StatementConverter::toCAPIStatement(stmt));
    if ( librdf_model_add_statement (_model->get(), librdfstmt.get()) ) {
        std::stringstream ss;
        ss << "Unable to add statement";
        throw InternalError(ss.str());
    }
}

void Model::remove(Statement *stmt) {
    std::shared_ptr<librdf_statement> librdfstmt(StatementConverter::toCAPIStatement(stmt));
    if ( librdf_model_remove_statement (_model->get(), librdfstmt.get()) ) {
        std::stringstream ss;
        ss << "Unable to remove statement";
        throw InternalError(ss.str());
    }
}

/**
 * Return one arc (predicate) of an arc in an RDF graph given source (subject) and arc (predicate).
 */
Node Model::findTarget(const Node& source, const Node& arc) const {
    return Node(librdf_model_get_target(_model->get(), source.get(), arc.get()), true);
}

#elif defined(USE_SORD)

Model::Model() : _world(new World()), _model(new ModelPrivate()) {
}

const char * guessFormat(const std::string& path) {
    std::string ext = path.substr(path.length() - 3);
    const char * format = nullptr;
    if ( ext == "ttl" ) {
        format = "turtle";
    } else if ( ext == ".nt" ) {
        format = "ntriples";
    }

    if ( !format ) {
        throw UnsupportedRdfFileFormat("Unable to deduce format from file save name");
    }

    return format;
}

void Model::loadFromFile(const std::string& path, const std::string& baseIRI) {
    const char *format = guessFormat(path);
    FILE *f = ::fopen(path.c_str(), "r");
    if ( !f ) {
        std::stringstream ss;
        ss << "Unable to open " << path << ": " << ::strerror(errno);
        throw FileIOError(ss.str().c_str());
    }
    try {
        loadFromFile(f, format, baseIRI, path);
    }
    catch(...) {
        ::fclose(f);
        throw;
    }
    ::fclose(f);
}

SerdSyntax getFormat(const std::string& format, const std::string& streamInfo) {
    std::map<std::string, SerdSyntax> syntax = {
            {"turtle", SERD_TURTLE},
            {"ntriples", SERD_NTRIPLES}
    };
    auto found = syntax.find(format);
    if ( found != syntax.end() ) {
        return found->second;
    } else {
        throw UnsupportedRdfFileFormat(streamInfo + ": File format not recognized");
    }
}

void extractBaseURI(Model *m, std::shared_ptr<SerdEnv> env, const std::string& baseIRI) {
    const SerdNode *readbase = serd_env_get_base_uri(env.get(), nullptr);
    if ( readbase && readbase->buf ) {
        m->setBaseUri(reinterpret_cast<const char*>(readbase->buf));
    } else {
        m->setBaseUri(baseIRI);
    }

    serd_env_foreach(env.get(), [](void * model, const SerdNode* name, const SerdNode* url) {
        Model *m = static_cast<Model*>(model);
        const char *nameStr = reinterpret_cast<const char*>(name->buf);
        if ( strlen(nameStr) ) {
            m->addNamespacePrefix(nameStr, reinterpret_cast<const char*>(url->buf));
        } else {
            m->setBaseUri(reinterpret_cast<const char*>(url->buf));
        }
        return SerdStatus::SERD_SUCCESS;
    }, m);
}

void Model::loadFromMemory(const void* data, const char *format, const std::string& baseIRI) {
    SerdNode base = serd_node_from_string(SERD_URI, reinterpret_cast<const uint8_t*>(baseIRI.c_str()));
    std::shared_ptr<SerdEnv> env = std::shared_ptr<SerdEnv>(serd_env_new(&base), &serd_env_free);

    SerdSyntax syntax = getFormat(format, "");

    std::shared_ptr<SerdReader> reader = std::shared_ptr<SerdReader>(sord_new_reader(_model->get(), env.get(), syntax, NULL), &serd_reader_free);
    serd_reader_read_string(reader.get(), reinterpret_cast<const uint8_t*>(data));

    extractBaseURI(this, env, baseIRI);
}

void Model::loadFromFile(FILE *fileHandle, const char *format, const std::string& baseIRI, const std::string& streamInfo) {
    SerdNode base = serd_node_from_string(SERD_URI, reinterpret_cast<const uint8_t*>(baseIRI.c_str()));
    std::shared_ptr<SerdEnv> env = std::shared_ptr<SerdEnv>(serd_env_new(&base), &serd_env_free);

    SerdSyntax syntax = getFormat(format, streamInfo);

    std::shared_ptr<SerdReader> reader = std::shared_ptr<SerdReader>(sord_new_reader(_model->get(), env.get(), syntax, NULL), &serd_reader_free);
    serd_reader_read_file_handle(reader.get(), fileHandle, reinterpret_cast<const uint8_t *>(streamInfo.c_str()));

    extractBaseURI(this, env, baseIRI);
}

void Model::saveToFile(const std::string& path, const std::string& baseIRI, bool enforceRepeatable, const char *format) {
    if ( !format ) {
        format = guessFormat(path);
    }
    FILE *f = ::fopen(path.c_str(), "w");
    if ( !f ) {
        std::stringstream ss;
        ss << "Unable to open " << path << ": " << ::strerror(errno);
        throw FileIOError(ss.str().c_str());
    }
    try {
        saveToFile(f, format, baseIRI, enforceRepeatable);
    }
    catch(...) {
        ::fclose(f);
        throw;
    }
    ::fclose(f);
}

SerdNode string2Node(SerdType type, const std::string& s) {
    SerdNode ret = {
            reinterpret_cast<const uint8_t*>(s.c_str()), s.length(), s.length(), 0, type };
    return ret;
}

void saveToWriter(Model *m, SordModel *sord, const char *format, const std::string& baseIRI, SerdSink sink, void *stream) {
    SerdNode base = serd_node_from_string(SERD_URI, reinterpret_cast<const uint8_t*>(baseIRI.c_str()));
    std::shared_ptr<SerdEnv> env = std::shared_ptr<SerdEnv>(serd_env_new(&base), &serd_env_free);

    SerdURI serd_uri = SERD_URI_NULL;
    if ( !baseIRI.empty() ) {
        if (serd_uri_parse(reinterpret_cast<const uint8_t*>(baseIRI.c_str()), &serd_uri)) {
            throw InvalidIRI("Invalid base URI");
        }
    }

    for ( auto nsPrefix : m->namespacesPrefixes() ) {
        const SerdNode name_node = string2Node(SERD_LITERAL, nsPrefix.first);
        const SerdNode uri_node  = string2Node(SERD_URI,     nsPrefix.second);
        serd_env_set_prefix(env.get(), &name_node, &uri_node);
    }

    SerdSyntax syntax = getFormat(format, "");
    std::shared_ptr<SerdWriter> writer = std::shared_ptr<SerdWriter>(
            serd_writer_new(syntax, SerdStyle(SERD_STYLE_CURIED | SERD_STYLE_ABBREVIATED | SERD_STYLE_RESOLVED), env.get(), &serd_uri, sink, stream),
            &serd_writer_free);

    const SerdNode baseNode = string2Node(SERD_URI, baseIRI);
    serd_writer_set_base_uri(writer.get(), &baseNode);
    serd_env_foreach(env.get(),
                     SerdPrefixSink(serd_writer_set_prefix),
                     writer.get());

    sord_write(sord, writer.get(), nullptr);
}

void Model::saveToFile(FILE *fileHandle, const char *format, const std::string& baseIRI, bool) {
    saveToWriter(this, _model->get(), format, baseIRI, serd_file_sink, fileHandle);
}

std::shared_ptr<std::string> Model::saveToMemory(const char *format, const std::string& baseIRI) {
    auto ret = std::make_shared<std::string>();
    saveToWriter(this, _model->get(), format, baseIRI, [](const void* buf, size_t len, void* stream) -> size_t {
        std::string* str = (std::string*)stream;
        str->append((const char*)buf, len);
        return len;
    }
    , ret.get());
    return ret;
}

std::list<std::string> Model::supportedFormat() const {
    return {"ntriples", "turtle"};
}

void Model::add(Statement *stmt) {
    SordQuad quad;
    StatementConverter::toCAPIStatement(stmt, &quad);
    if ( !sord_contains(_model->get(), quad) ) {
        if ( !sord_add(_model->get(), quad) ) {
            std::stringstream ss;
            ss << "Unable to add statement: " << *stmt;
            throw InternalError(ss.str());
        }
    }
}

void Model::remove(Statement *stmt) {
    SordQuad quad;
    StatementConverter::toCAPIStatement(stmt, &quad);
    if ( sord_contains(_model->get(), quad) ) {
        sord_remove(_model->get(), quad);
    } else {
        std::stringstream ss;
        ss << "Unexisting statement";
        throw InternalError(ss.str());
    }
}

/**
 * Return one arc (predicate) of an arc in an RDF graph given source (subject) and arc (predicate).
 */
Node Model::findTarget(const Node& source, const Node& arc) const {
    return Node(sord_get(_model->get(), source.get(), arc.get(), nullptr, nullptr), true);
}

#endif

std::string Model::genBlankNodeId() const {
    return _world->genBlankNodeId();
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

/**ndif
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
 * Return the properties pointing to the given resource.
 */
NodeList Model::arcsIn(const Node& node) const {
    return NodeList(node, NodeList::Mode::ARCSIN, this);
}

/**
 * Return the properties pointing from the given resource.
 */
NodeList Model::arcsOut(const Node& node) const {
    return NodeList(node, NodeList::Mode::ARCSOUT, this);
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
        if ( rdfiri.find(iri) == 0 ) {
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
