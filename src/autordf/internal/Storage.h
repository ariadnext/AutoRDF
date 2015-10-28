#ifndef AUTORDF_STORAGE_H
#define AUTORDF_STORAGE_H

#include <librdf.h>

namespace autordf {
namespace internal {

/* Default in memory storage */
class Storage {
public:
    Storage();

    Storage(const Storage&) = delete;

    ~Storage();

    librdf_storage* get() const { return _storage; }

private:
    librdf_storage* _storage;
};

}
}

#endif //AUTORDF_STORAGE_H
