#include "autordf/World.h"

#include <cstdio>
#include <iostream>

namespace autordf {

std::mutex World::_mutex;
librdf_world* World::_world;
int World::_refcount;

World::World() {
    std::lock_guard<std::mutex> locker(_mutex);
    if (!_world) {
        _world = librdf_new_world();
        librdf_world_open(_world);

        //librdf_world_set_logger(_world, NULL, logCB);
        librdf_world_set_warning(_world, NULL, warnCB);
        librdf_world_set_error(_world, NULL, errorCB);
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

//FIXME
int World::logCB(void *user_data, librdf_log_message *message) {
    std::cerr << "Redland INFO: "<< message << std::endl;
    return 1;
}

int World::warnCB(void *user_data, const char *message, va_list arguments) {
    char text[255];
    vsnprintf(text, sizeof(text), message, arguments);
    text[sizeof(text) - 1] = 0;
    std::cerr << "Redland WARN: "<< text << std::endl;
    return 1;
}

int World::errorCB(void *user_data, const char *message, va_list arguments) {
    char text[255];
    vsnprintf(text, sizeof(text), message, arguments);
    text[sizeof(text) - 1] = 0;
    throw std::runtime_error(std::string("Redland ERROR: ") + text);
    return 1;
}

}
