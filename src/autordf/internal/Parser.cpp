#ifdef USE_REDLAND
#include "autordf/internal/Parser.h"

#include "autordf/internal/World.h"
#include "autordf/internal/Uri.h"
#include "autordf/Exception.h"

namespace autordf {
namespace internal {

Parser::Parser(const std::string& name) {
    _parser = librdf_new_parser(World().get(), name.c_str(), NULL, NULL);
    if (!_parser) {
        throw InternalError("Failed to construct RDF parser");
    }
}

Parser::~Parser() {
    librdf_free_parser(_parser);
    _parser = 0;
}

}
}
#endif