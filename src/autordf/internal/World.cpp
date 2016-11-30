#include "autordf/internal/cAPI.h"
#include "autordf/internal/World.h"

#include <cstdio>
#include <cstdlib>
#include <sys/time.h>
#include <unistd.h>
#include <sstream>
#include <iostream>

#include "autordf/Exception.h"

namespace autordf {
namespace internal {

std::mutex World::_mutex;
c_api_world* World::_world;
int World::_refcount;

#if defined(USE_REDLAND)
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

std::string World::genBlankNodeId() const {
    unsigned char * genid = librdf_world_get_genid(_world);
    std::string id = reinterpret_cast<const char *>(genid);
    free(genid);
    return id;
}

int World::logCB(void*, librdf_log_message* message) {
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
        ::exit(1);
    }

    /* Handled */
    return 1;
}

#elif defined(USE_SORD)

unsigned long World::_genIdBase;
unsigned long World::_genIdCtr;

World::World() {
    std::lock_guard<std::mutex> locker(_mutex);
    if (!_world) {
        _world = sord_world_new();
        sord_world_set_error_sink(_world, sordErrorCB, nullptr);
        struct timeval tv;
        if (!gettimeofday(&tv, nullptr)) {
            _genIdBase = (unsigned long)tv.tv_sec;
        }
    }
    ++_refcount;
}

World::~World() {
    std::lock_guard<std::mutex> locker(_mutex);
    if (--_refcount == 0) {
        sord_world_free(_world);
        _world = 0;
    }
}

SerdStatus World::sordErrorCB(void*, const SerdError* error) {
    fprintf(stderr, "Serd ERROR - ");
    vfprintf(stderr, error->fmt, *const_cast<va_list *>(error->args));
    fprintf(stderr, "\n");
    ::exit(1);
}

std::string World::genBlankNodeId() const {
    unsigned long pid = (unsigned long)getpid();
    std::stringstream id;
    id << 'r' << _genIdBase << 'r' << pid << 'r' << ++_genIdCtr;
    return id.str();
}

#endif

}
}
