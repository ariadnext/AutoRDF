#ifndef AUTORDF_TRIPLESTORE_H
#define AUTORDF_TRIPLESTORE_H

#include <string>
#include <memory>
#include <map>

#include <autordf/StatementList.h>

namespace autordf {

class World;
class ModelPrivate;
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
     * @throw FileIOError is fil does not exist
     * @throw InternalError
     */
    void loadFromFile(const std::string& path, const std::string& baseIRI);

    /** Save model to file.
     * If no format is supplied, auto-detection is guessed
     * from provided file name
     * @throw UnsupportedRdfFileFormat if format is not recognized
     * @throw InternalError if for some strange reason data serialization failed
     */
    void saveToFile(const std::string& path, const std::string& baseUri = "", const char *format = 0);

    /**
     * Search for statements in model
     *
     * If no filter is given, gives back all elements from model
     */
    StatementList find(const Statement& filter = Statement());

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
     * Returns the list of namespaces known in this model in a map prefix --< IRI
     */
    const std::map<std::string, std::string>& namespacesPrefixes() const { return _namespacesPrefixes; }

    /**
     * Adds given prefix to prefix mapping table
     * @throw InternalError If prefix is already registered to different namespace,
     */
    void addNamespacePrefix(const std::string& prefix, const std::string& ns);

protected:
    /**
     * A pointer to the librdf internal world structure
     */
    std::shared_ptr<World> _world;

private:
    std::shared_ptr<ModelPrivate> _model;
    // What is it exactly ?
    std::string _baseUri;
    // Prefixes seen during parsing prefix --> IRI
    std::map<std::string, std::string> _namespacesPrefixes;

    friend class StatementList;
};

}

#endif //AUTORDF_TRIPLESTORE_H
