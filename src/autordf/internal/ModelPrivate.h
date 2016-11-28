#ifndef AUTORDF_MODELPRIVATE_H
#define AUTORDF_MODELPRIVATE_H

#include <memory>
#include <autordf/internal/cAPI.h>

namespace autordf {

class Storage;

namespace internal {

class ModelPrivate {
public:
#if defined(USE_REDLAND)
    ModelPrivate(std::shared_ptr<Storage> storage);
#elif defined(USE_SORD)
    ModelPrivate();
#endif

    ~ModelPrivate();

    c_api_model *get() const { return _model; }

private:
    c_api_model *_model;
#if defined(USE_REDLAND)
    std::shared_ptr<Storage> _storage;
#endif
};

}
}

#endif //AUTORDF_MODELPRIVATE_H
