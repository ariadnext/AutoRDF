#include "autordf/Uri.h"

#include "autordf/World.h"

namespace autordf {

Uri::Uri(const std::string& uri) {
    _uri = librdf_new_uri(World().get(), reinterpret_cast<const unsigned char *>(uri.c_str()));
    if ( !_uri ) {
        throw std::runtime_error("Failed to create URI");
    }
}

Uri::~Uri() {
    librdf_free_uri(_uri);
    _uri = 0;
}

}