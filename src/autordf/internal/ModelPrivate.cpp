#include "autordf/internal/ModelPrivate.h"

#include "autordf/internal/World.h"
#include "autordf/internal/Storage.h"
#include "autordf/Exception.h"

namespace autordf {
namespace internal {

ModelPrivate::ModelPrivate(std::shared_ptr<Storage> storage) : _storage(storage) {
    /* Default storage type, which is memory */
    _model = librdf_new_model(World().get(), storage->get(), NULL);
    if (!_model) {
        throw InternalError("Failed to create RDF model");
    }
}

ModelPrivate::~ModelPrivate() {
    librdf_free_model(_model);
    _model = 0;
}

}
}
