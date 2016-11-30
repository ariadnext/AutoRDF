#ifndef AUTORDF_TRIPLESTORE_H
#define AUTORDF_TRIPLESTORE_H

#include <string>
#include <memory>
#include <map>
#include <list>

#include <autordf/StatementList.h>
#include <autordf/NodeList.h>
#ifdef USE_REDLAND
#include <autordf/Storage.h>
#endif

#include <cstdlib>

namespace autordf {

namespace internal {
class World;

class ModelPrivate;

class Parser;
}
class StatementList;

/**
 * Model is the Entry point class for reading files using librdf
 *
 * It provides basic load/save and statement search facility
 */
class Model {
public:
    /**
     * Builds a new Model instance, using in-memory storage
     */
    Model();

#ifdef USE_REDLAND
    /**
     * Builds a new Model instance, using specified storage
     */
    Model(std::shared_ptr<Storage>);
#endif

    /**
     * Disallow copy construction
     */
    Model(const Model&) = delete;

    /**
     * Loads rdf resource from a local file
     * @param path: where to load data from
     * @param baseIRI: prefix for prefix-less data
     * @throw UnsupportedRdfFileFormat if format is not recognized
     * @throw FileIOError is file does not exist
     * @throw InternalError
     */
    void loadFromFile(const std::string& path, const std::string& baseIRI = "");

    /**
     * Loads rdf resource from a string
     * @param data: the memory buffer to read from
     * @param format: fileformat to use. Name of a redland parser
     * @param baseIRI: prefix for prefix-less data
     * @throw UnsupportedRdfFileFormat if format is not recognized
     * @throw FileIOError is file does not exist
     * @throw InternalError
     */
    void loadFromMemory(const void* data, const char *format, const std::string& baseIRI = "");

    /**
     * Loads rdf resource from a local file
     * @param fileHandle: file handle where to load data from - open with fopen()
     * @param format: fileformat to use. Name of a redland parser
     * @param baseIRI: prefix for prefix-less data
     * @param streamInfo: Usually name of the file, used only to output more precise exceptions text in case of error
     * @throw UnsupportedRdfFileFormat if format is not recognized
     * @throw InternalError
     */
    void loadFromFile(FILE *fileHandle, const char *format, const std::string& baseIRI = "", const std::string& streamInfo = "<unknown stream>");

    /**
     * Save model to file.
     * If no format is supplied, auto-detection is guessed
     * from provided file name
     * @param path filename to write file to
     * @param baseIRI if not empty, serializer will express all iris relatively to this one
     * @param enforceRepeatable if not set (default), the ordering of nodes might not be reproducible from on save to another.
     * This parameter should be set to true if file is to be saved in a source code version management system. It should only
     * be used with small models, as all statements are loaded and then sorted in memory
     * @param format fileformat to use. If not given will autodetect from file path extension
     * @throw UnsupportedRdfFileFormat if format is not recognized
     * @throw InternalError if for some strange reason data serialization failed
     */
    void saveToFile(const std::string& path, const std::string& baseIRI = "", bool enforceRepeatable = false, const char *format = 0);

    /**
     * Save model to file.
     * If no format is supplied, auto-detection is guessed
     * from provided file name
     * @param fileHandle: file handle where to write data to - open with fopen()
     * @param format fileformat to use. If empty use default RDF/XML format
     * @param baseIRI if not empty, serializer will express all iris relatively to this one
     * @param enforceRepeatable if not set (default), the ordering of nodes might not be reproducible from on save to another
     * This parameter should be set to true if file is to be saved in a source code version management system. It should only
     * be used with small models, as all statements are loaded and then sorted in memory
     * @throw UnsupportedRdfFileFormat if format is not recognized
     * @throw InternalError if for some strange reason data serialization failed
     */
    void saveToFile(FILE *fileHandle, const char *format, const std::string& baseIRI = "", bool enforceRepeatable = false);

    /**
     * Saves model into memory
     *
     * @param format fileformat to use. If empty use default RDF/XML format
     * @param baseIRI if not empty, serializer will express all iris relatively to this one
     * @throw UnsupportedRdfFileFormat if format is not recognized
     * @throw InternalError if for some strange reason data serialization failed
     */
    std::shared_ptr<std::string> saveToMemory(const char *format = "", const std::string& baseIRI = "");

    /**
     * Retrieve supported output format for use with saveToFile
     */
    std::list<std::string> supportedFormat() const;

    /**
     * Search for statements in model
     *
     * If no filter is given, gives back all elements from model
     */
    StatementList find(const Statement& filter = Statement()) const;

    /**
     * Return the sources (subjects) of arc in an RDF graph given arc (predicate) and target (object).
     */
    NodeList findSources(const Node& arc, const Node& target) const;

    /**
     * Return the arcs (predicates) of an arc in an RDF graph given source (subject) and target (object).
     */
    NodeList findArcs(const Node& source, const Node& target) const;

    /**
     * Return the targets (objects) of an arc in an RDF graph given source (subject) and arc (predicate).
     */
    NodeList findTargets(const Node& source, const Node& arc) const;

    /**
     * Return the properties pointing to the given resource.
     */
    NodeList arcsIn(const Node& node) const;

    /**
     * Return the properties pointing from the given resource.
     */
    NodeList arcsOut(const Node& node) const;

    /**
     * Return one arc (predicate) of an arc in an RDF graph given source (subject) and arc (predicate).
     */
    Node findTarget(const Node& source, const Node& arc) const;

    /**
     * Adds a statement to model
     * When this function returns, stmt is erased
     * @throw InternalError on issue
     */
    void add(Statement* stmt);

    /**
     * Removes a statement from model
     * When this function returns, stmt is erased
     * @throw InternalError on issue
     */
    void remove(Statement* stmt);

    /**
     * Maps a XML namespace to its prefix
     * @throw std::out_of_range if not found
     */
    const std::string& nsToPrefix(const std::string& ns) const;

    /**
     * Maps a XML prefix to its namespace
     * @throw std::out_of_range if not found
     */
    const std::string& prefixToNs(const std::string& prefix) const;

    /**
     * Return the base Uri
     */
    const std::string& baseUri() const { return _baseUri; }

    /**
     * Return the base Uri
     */
    void setBaseUri(const std::string& baseUri) { _baseUri = baseUri; }

    /**
     * Returns the list of namespaces known in this model in a prefix --> IRI map
     */
    const std::map<std::string, std::string>& namespacesPrefixes() const { return _namespacesPrefixes; }

    /**
     * Adds given prefix to prefix mapping table
     * @throw InternalError If prefix is already registered to different namespace,
     */
    void addNamespacePrefix(const std::string& prefix, const std::string& ns);

    /**
     * Returns the prefix that matches the given rdfiri if a prefix is registered, empty otherwise
     */
    std::string iriPrefix(const std::string& rdfiri) const;

protected:
    /**
     * A pointer to the librdf internal world structure
     */
    std::shared_ptr<internal::World> _world;

    std::string genBlankNodeId() const;

private:
    std::shared_ptr<internal::ModelPrivate> _model;
    // What is it exactly ?
    std::string _baseUri;
    // Prefixes seen during parsing prefix --> IRI
    std::map<std::string, std::string> _namespacesPrefixes;

    friend class StatementList;
    friend class NodeList;
};

}

#endif //AUTORDF_TRIPLESTORE_H
