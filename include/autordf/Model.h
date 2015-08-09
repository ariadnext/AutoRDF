#ifndef AUTORDF_TRIPLESTORE_H
#define AUTORDF_TRIPLESTORE_H

#include <string>
#include <memory>

#include <autordf/StatementList.h>

namespace autordf {

class World;
class ModelPrivate;

/**
 * Model ins the Entry point class for reading files using autordf
 */
class Model {
public:
    Model();
    Model(const Model&) = delete;

    // Loads rdf resource from a local file
    void loadFromFile(const std::string& path);

    // Search for statements in model
    // If no filter is given, gives back all elements from model
    StatementList find(const Statement& filter = Statement());

private:
    std::shared_ptr<World> _world;
    std::shared_ptr<ModelPrivate> _model;
};

}

#endif //AUTORDF_TRIPLESTORE_H
