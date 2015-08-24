#include "autordf/ModelPrivate.h"

#include "autordf/World.h"
#include "autordf/Storage.h"
#include "autordf/Exception.h"

namespace autordf {

ModelPrivate::ModelPrivate(std::shared_ptr<Storage> storage) :_storage(storage) {
    /* Default storage type, which is memory */
    _model = librdf_new_model(World().get(), storage->get(), NULL);
    if ( !_model ) {
        throw InternalError("Failed to create RDF model");
    }
}

ModelPrivate::~ModelPrivate() {
    librdf_free_model( _model );
    _model = 0;
}

}
