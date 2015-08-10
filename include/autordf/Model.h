#ifndef AUTORDF_TRIPLESTORE_H
#define AUTORDF_TRIPLESTORE_H

#include <string>
#include <memory>

#include <autordf/StatementList.h>

namespace autordf {

class World;
class ModelPrivate;
class StatementList;

/**
 * Model ins the Entry point class for reading files using autordf
 */
class Model {
public:
    Model();
    Model(const Model&) = delete;

    // Loads rdf resource from a local file
    // @param baseIRI: prefix for prefix-less data
    void loadFromFile(const std::string& path, const std::string& baseIRI);

    // Save model to file.
    // If no format is supplied, auto-detection is guessed
    // from provided file name
    void saveToFile(const std::string& path, const char *format = 0);

    // Search for statements in model
    // If no filter is given, gives back all elements from model
    StatementList find(const Statement& filter = Statement());

    // Adds a statement to model
    // throws an exception on error;
    void add(const Statement& stmt);

    // Removes a statement from model
    // throws an exception on error;
    void remove(const Statement& stmt);

protected:
    std::shared_ptr<World> _world;

private:
    std::shared_ptr<ModelPrivate> _model;
    std::string _baseUri;

    friend class StatementList;
};

}

#endif //AUTORDF_TRIPLESTORE_H
