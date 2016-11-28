#ifdef USE_REDLAND
#include <autordf/internal/cAPI.h>
#include <autordf/Storage.h>

#include "autordf/internal/World.h"
#include "autordf/Exception.h"

namespace autordf {

Storage::Storage() : _world(new internal::World()) {
    /* Default storage type, which is memory hash */
    _storage = librdf_new_storage(_world->get(), "hashes", nullptr, "hash-type='memory'");
    if (!_storage) {
        throw InternalError("Failed to create RDF data storage");
    }
}

Storage::Storage(const std::string& storageName, const std::string& name, const std::string& optionsString)
        : _world(new internal::World()) {
    _storage = librdf_new_storage(_world->get(), storageName.c_str(), name.c_str(), optionsString.c_str());
    if (!_storage) {
        throw InternalError("Failed to create RDF data storage");
    }
}

Storage::~Storage() {
    librdf_free_storage(_storage);
    _storage = 0;
}
}
#endif

