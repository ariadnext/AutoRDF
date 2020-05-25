#include "autordf/internal/ModelPrivate.h"

#include "autordf/internal/World.h"
#include "autordf/Storage.h"
#include "autordf/Exception.h"

namespace autordf {
namespace internal {

#if defined(USE_REDLAND)
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
#elif defined(USE_SORD)
ModelPrivate::ModelPrivate() {
    /* Default storage type, which is memory */

#if defined(NATIVE)
    _model = sord_new(World().get(), 0xFF, false);
#else
    /*
      will build SPO(default) and OPS only index
    */
    _model = sord_new(World().get(), 0x4, false);
#endif
    if (!_model) {
        throw InternalError("Failed to create RDF model");
    }
}

ModelPrivate::~ModelPrivate() {
    sord_free(_model);
    _model = 0;
}
#endif

}
}
