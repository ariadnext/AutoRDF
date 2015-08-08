#ifndef AUTORDF_TRIPLESTORE_H
#define AUTORDF_TRIPLESTORE_H

#include <string>
#include <memory>

#include <autordf/StatementList.h>

namespace autordf {

class World;
class ModelPrivate;

class Model {
public:
    Model();
    Model(const Model&) = delete;

    void loadFromFile(const std::string& path);

    StatementList find();

private:
    std::shared_ptr<World> _world;
    std::shared_ptr<ModelPrivate> _model;
};

}

#endif //AUTORDF_TRIPLESTORE_H
