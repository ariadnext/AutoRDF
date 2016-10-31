#ifndef AUTORDF_WORLD_H
#define AUTORDF_WORLD_H

#include <cstdio>
#include <mutex>

#ifdef LIBRDF_IN_SUBDIRS
#include <librdf/librdf.h>
#else
#include <librdf.h>
#endif

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

    librdf_world* get() const { return _world; }

private:
    static std::mutex _mutex;
    static librdf_world* _world;
    static int _refcount;

    static int logCB(void* user_data, librdf_log_message* message);
};

}
}

#endif //AUTORDF_WORLD_H
