#ifndef AUTORDF_MODELPRIVATE_H
#define AUTORDF_MODELPRIVATE_H

#ifdef LIBRDF_IN_SUBDIRS
#include <librdf/librdf.h>
#else
#include <librdf.h>
#endif

#include <memory>

namespace autordf {

class Storage;

namespace internal {

class ModelPrivate {
public:
    ModelPrivate(std::shared_ptr<Storage> storage);

    ~ModelPrivate();

    librdf_model *get() const { return _model; }

private:
    librdf_model *_model;
    std::shared_ptr<Storage> _storage;
};

}
}

#endif //AUTORDF_MODELPRIVATE_H
