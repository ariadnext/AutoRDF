#ifndef AUTORDF_MODELPRIVATE_H
#define AUTORDF_MODELPRIVATE_H

#include <librdf.h>

#include <memory>

namespace autordf {

class Storage;

class ModelPrivate {
public:
    ModelPrivate(std::shared_ptr<Storage> storage);
    ~ModelPrivate();

    librdf_model * get() const { return _model; }
private:
    librdf_model *_model;
    std::shared_ptr<Storage> _storage;
};

}

#endif //AUTORDF_MODELPRIVATE_H
