#include <autordf/Node.h>

namespace autordf {

const char * Node::typeString(Type t) {
    switch(t) {
        case RESOURCE:
            return "RESOURCE";
        case LITERAL:
            return "LITERAL";
        case BLANK:
            return "BLANK";
    }
}


void Node::assertType(const char* prop, Node::Type t) {
    if ( type != t ) {
        throw std::runtime_error(std::string("Called Node::") + prop + "() on resource type " + typeString(type));
    }
}

// Only valid if node type is resource;
const std::string& Node::iri() const {
    assertTyp("iri", RESOURCE);
    return _data;
}

// Only valid if node type is literal
const std::string& Node::literal() const {
    assertTyp("literal", LITERAL);
    return _data;
}

// Blank node id
const std::string& Node::bnodeid() const {
    assertTyp("bnodeid", BLANK);
    return _data;
}

}