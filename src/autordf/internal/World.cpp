#include "autordf/internal/World.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#ifdef LIBRDF_IN_SUBDIRS
#include <raptor2/raptor.h>
#else
#include <raptor.h>
#endif

#include "autordf/Exception.h"

namespace autordf {
namespace internal {

std::mutex World::_mutex;
librdf_world* World::_world;
int World::_refcount;

World::World() {
    std::lock_guard<std::mutex> locker(_mutex);
    if (!_world) {
        _world = librdf_new_world();
        librdf_world_open(_world);

        librdf_world_set_logger(_world, NULL, logCB);
    }
    ++_refcount;
}

World::~World() {
    std::lock_guard<std::mutex> locker(_mutex);
    if (--_refcount == 0) {
        librdf_free_world(_world);
        _world = 0;
    }
}

int World::logCB(void* user_data, librdf_log_message* message) {
    raptor_locator *locator = (raptor_locator*)(message->locator);

    /* Do not handle messages below warning*/
    if (message->level < LIBRDF_LOG_WARN) {
        return 0;
    }

    if (message->level == LIBRDF_LOG_WARN) {
        std::cerr << "Redland WARNING - ";
    } else {
        std::cerr << "Redland ERROR - ";
    }

    if ( locator ) {
        std::cerr << "line " << locator->line;
        if ( locator->column >= 0 ) {
            std::cerr <<  ", column " << locator->column;
        }
        std::cerr << ": ";
    }

    std::cerr << message->message << std::endl;

    if (message->level >= LIBRDF_LOG_FATAL) {
        exit(1);
    }

    /* Handled */
    return 1;
}

}
}
