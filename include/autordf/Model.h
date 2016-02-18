#ifndef AUTORDF_TRIPLESTORE_H
#define AUTORDF_TRIPLESTORE_H

#include <string>
#include <memory>
#include <map>

#include <autordf/StatementList.h>
#include <autordf/NodeList.h>

#include <cstdlib>

namespace autordf {

namespace internal {
class World;

class ModelPrivate;
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
     * Builds a new Model instance
     */
    Model();

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
    void loadFromFile(const std::string& path, const std::string& baseIRI = ".");

    /**
     * Loads rdf resource from a local file
     * @param fileHandle: file handle where to load data from - open with fopen()
     * @param format: extension of the file
     * @param baseIRI: prefix for prefix-less data
     * @throw UnsupportedRdfFileFormat if format is not recognized
     * @throw InternalError
     */
    void loadFromFile(FILE *fileHandle, const std::string& format, const std::string& baseIRI = ".");

    /**
     * Save model to file.
     * If no format is supplied, auto-detection is guessed
     * from provided file name
     * @param path filename to write file to
     * @param baseIRI if not empty, serializer will express all iris relatively to this one
     * @param format fileformat to use. If not given will autodetect from file path extension
     * @throw UnsupportedRdfFileFormat if format is not recognized
     * @throw InternalError if for some strange reason data serialization failed
     */
    void saveToFile(const std::string& path, const std::string& baseIRI = "", const char *format = 0);

    /**
     * Save model to file.
     * If no format is supplied, auto-detection is guessed
     * from provided file name
     * @param fileHandle: file handle where to write data to - open with fopen()
     * @param format fileformat to use. If empty use default RDF/XML format
     * @param baseIRI if not empty, serializer will express all iris relatively to this one
     * @throw UnsupportedRdfFileFormat if format is not recognized
     * @throw InternalError if for some strange reason data serialization failed
     */
    void saveToFile(FILE *fileHandle, const char *format = "", const std::string& baseIRI = "");

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
     * Return one arc (predicate) of an arc in an RDF graph given source (subject) and target (object).
     */
    Node findTarget(const Node& source, const Node& arc) const;

    /**
     * Adds a statement to model
     * @throw InternalError on issue
     */
    void add(const Statement& stmt);

    /**
     * Removes a statement from model
     * @throw InternalError on issue
     */
    void remove(const Statement& stmt);

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

private:
    std::shared_ptr<internal::ModelPrivate> _model;
    // What is it exactly ?
    std::string _baseUri;
    // Prefixes seen during parsing prefix --> IRI
    std::map<std::string, std::string> _namespacesPrefixes;

    // Remove any # : / at the end of strint
    static std::string cleanNamespaceTrailingChars(std::string ns);

    friend class StatementList;
    friend class NodeList;
};

}

#endif //AUTORDF_TRIPLESTORE_H
