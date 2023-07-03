#ifndef AUTORDF_WORLD_H
#define AUTORDF_WORLD_H

#include <cstdio>
#include <mutex>
#include <string>
#include <autordf/internal/cAPI.h>

namespace autordf {
namespace internal {

/**
 * This class holds a static singleton on librdf world object.
 * Singleton is created upon first time class instanciation, and
 * is deleted when last instance of class is destroyed
 */
class World {
public:
    World();

    World(const World&) = delete;

    ~World();

    c_api_world* get() const { return _world; }

    /**
     * Generates a new, unique, id
     * Can be used as blank node id
     * @return unqiue id
     */
    static std::string genUniqueId();

private:
    static std::mutex _mutex;
    static c_api_world* _world;
    static int _refcount;
#if defined(USE_REDLAND)
    static int logCB(void* user_data, librdf_log_message* message);
#endif
#if defined(USE_SORD)
    static unsigned long _genIdBase;
    static unsigned long _genIdCtr;
    static SerdStatus sordErrorCB(void* handle, const SerdError* error);
#endif
};

}
}

#endif //AUTORDF_WORLD_H
