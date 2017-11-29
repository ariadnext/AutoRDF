#ifndef AUTORDF_STORAGE_H
#define AUTORDF_STORAGE_H

#include <string>
#include <memory>

#include <autordf/autordf_export.h>

typedef struct librdf_storage_s librdf_storage;

namespace autordf {

namespace internal {
class World;
}

/* Implements a storage engine for rdf database */
class Storage {
public:
    /**
     * Create a in-memory hash storage.
     **/
    AUTORDF_EXPORT Storage();

    /**
     * Create a custom storage
     *
     * @see http://librdf.org/docs/api/redland-storage-modules.html
     *
     * @param storageName the storage factory name
     * @param name an identifier for the storage
     * @param optionsString options to initialise storage
     */
    AUTORDF_EXPORT Storage(const std::string& storageName, const std::string& name, const std::string& optionsString) ;

    AUTORDF_EXPORT Storage(const Storage&) = delete;

    ~Storage();

    librdf_storage* get() const { return _storage; }

private:
    librdf_storage* _storage;

    /**
     * A pointer to the librdf internal world structure
     */
    std::shared_ptr<internal::World> _world;
};

}

#endif //AUTORDF_STORAGE_H
