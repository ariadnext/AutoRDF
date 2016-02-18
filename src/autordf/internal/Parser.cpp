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

std::shared_ptr<Parser> Parser::guessFromExtension(const std::string& format) {
    const char* name = librdf_parser_guess_name2(World().get(), NULL, NULL, (const unsigned char *)(format.c_str()));
    if (name) {
        return std::make_shared<Parser>(name);
    } else {
        return 0;
    }
}

}
}