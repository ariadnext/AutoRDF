#ifndef AUTORDF_NODE_H
#define AUTORDF_NODE_H

#include <string>

namespace autordf {

class Node {
public:
    enum Type {
        RESOURCE,
        LITERAL,
        BLANK
    };
    Type type;
    static const char * typeString(Type t);

    // Only valid if node type is resource;
    const std::string& iri() const;
    // Only valid if node type is literal
    const std::string& literal() const;
    // Blank node id
    const std::string& bnodeid() const;
private:
    std::string _value;

    // Makes sure we are right type when accessing dedicated property
    void assertType(const std::string& prop, Type t);

    friend class StatementConverter;
};

}

#endif //AUTORDF_NODE_H
