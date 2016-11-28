#ifndef AUTORDF_PARSER_H
#define AUTORDF_PARSER_H

#include <string>
#include <memory>

#include <autordf/internal/cAPI.h>

namespace autordf {
namespace internal {

class Parser {
public:
    Parser(const std::string& name);

    Parser(const Parser&) = delete;

    ~Parser();

    librdf_parser* get() const { return _parser; }

private:
    librdf_parser* _parser;
};

}
}

#endif //AUTORDF_PARSER_H
