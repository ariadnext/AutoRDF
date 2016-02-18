#ifndef AUTORDF_PARSER_H
#define AUTORDF_PARSER_H

#include <librdf.h>

#include <string>
#include <memory>

namespace autordf {
namespace internal {

/* Default in memory storage */
class Parser {
public:
    Parser(const std::string& name);

    Parser(const Parser&) = delete;

    ~Parser();

    static std::shared_ptr<Parser> guessFromExtension(const std::string& format);

    librdf_parser* get() const { return _parser; }

private:
    librdf_parser* _parser;
};

}
}

#endif //AUTORDF_PARSER_H
