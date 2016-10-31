#ifndef AUTORDF_PARSER_H
#define AUTORDF_PARSER_H

#ifdef LIBRDF_IN_SUBDIRS
#include <librdf/librdf.h>
#else
#include <librdf.h>
#endif

#include <string>
#include <memory>

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
