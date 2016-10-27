#ifndef AUTORDF_STORAGE_H
#define AUTORDF_STORAGE_H

#include <string>

typedef struct librdf_storage_s librdf_storage;

namespace autordf {

/* Implements a storage engine for rdf database */
class Storage {
public:
    /**
     * Create a in-memory hash storage.
     **/
    Storage();

    /**
     * Create a custom storage
     *
     * @see http://librdf.org/docs/api/redland-storage-modules.html
     *
     * @param storageName the storage factory name
     * @param name an identifier for the storage
     * @param optionsString options to initialise storage
     */
    Storage(const std::string& storageName, const std::string& name, const std::string& optionsString) ;

    Storage(const Storage&) = delete;

    ~Storage();

    librdf_storage* get() const { return _storage; }

private:
    librdf_storage* _storage;
};

}

#endif //AUTORDF_STORAGE_H
