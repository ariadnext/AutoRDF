#include "autordf/internal/cAPI.h"
#include "autordf/internal/World.h"

#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <iostream>

#include <boost/date_time.hpp>
//#include <boost/config.hpp>
#ifndef WIN32
#include <unistd.h>
#endif

// include Windows.h after boost because otherwise I get
// fatal error C1189: #error:  WinSock.h already included
#ifdef WIN32
#include <Windows.h>
#endif

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
    try {
        std::lock_guard<std::mutex> locker(_mutex);
        if (--_refcount == 0) {
            librdf_free_world(_world);
            _world = 0;
        }
    } catch(const std::system_error& e) {
        std::cerr << "World::~World() unable to acquire lock" << std::endl;
    }
}

std::string World::genUniqueId() const {
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
    using namespace boost::gregorian;
    using namespace boost::local_time;
    using namespace boost::posix_time;

    std::lock_guard<std::mutex> locker(_mutex);
    if (!_world) {
        _world = sord_world_new();
        sord_world_set_error_sink(_world, sordErrorCB, nullptr);
        ptime now = second_clock::local_time();
        ptime time_t_epoch(date(1970,1,1));
        time_duration diff = now - time_t_epoch;

        _genIdBase = diff.total_seconds();
    }
    ++_refcount;
}

World::~World() {
    try {
        std::lock_guard<std::mutex> locker(_mutex);
        if (--_refcount == 0) {
            sord_world_free(_world);
            _world = 0;
        }
    } catch(const std::system_error& e) {
        std::cerr << "World::~World() unable to acquire lock" << std::endl;
    }
}

SerdStatus World::sordErrorCB(void*, const SerdError* error) {
    fprintf(stderr, "Serd ERROR - ");
    vfprintf(stderr, error->fmt, *const_cast<va_list *>(error->args));
    fprintf(stderr, "\n");
    ::exit(1);
}

std::string World::genUniqueId() {
// boost/process/environment.hpp generates too many errors on WINRT and isn't available until boost 1.64
// so use POSIX / Windows methods instead of boost::this_process::get_id()
#ifdef WIN32
    unsigned long pid = GetCurrentProcessId();
#else
    pid_t pid = getpid();
#endif
    std::stringstream id;
    id << 'B' << _genIdBase << 'b' << pid << 'b' << ++_genIdCtr;
    return id.str();
}

#endif

}
}
