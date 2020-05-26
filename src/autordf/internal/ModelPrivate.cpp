#include "autordf/internal/ModelPrivate.h"

#include "autordf/internal/World.h"
#include "autordf/Storage.h"
#include "autordf/Exception.h"

static unsigned int sord_init_flag = 0xFF;

namespace autordf {

  void set_sord_init_flag(unsigned int f) {
    sord_init_flag = f;
  }
  
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

    _model = sord_new(World().get(), sord_init_flag, false);
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
