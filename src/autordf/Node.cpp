#include <autordf/Node.h>

#include <stdexcept>

namespace autordf {

const char * Node::typeString(Type t) {
    switch(t) {
        case RESOURCE:
            return "RESOURCE";
        case LITERAL:
            return "LITERAL";
        case BLANK:
            return "BLANK";
        case EMPTY:
            return "EMPTY";
    }
}

void Node::assertType(const char* prop, Node::Type t) const {
    if ( type != t ) {
        throw std::runtime_error(std::string("Called Node::") + prop + "() on resource type " + typeString(type));
    }
}

// Only valid if node type is resource;
const std::string& Node::iri() const {
    assertType("iri", RESOURCE);
    return _value;
}

// Only valid if node type is literal
const std::string& Node::literal() const {
    assertType("literal", LITERAL);
    return _value;
}

// Blank node id
const std::string& Node::nodeId() const {
    assertType("nodeId", BLANK);
    return _value;
}

std::ostream& operator<<(std::ostream& os, const Node& n) {
    switch(n.type) {
        case Node::RESOURCE:
            os << "R";
            break;
        case Node::LITERAL:
            os << "L";
            break;
        case Node::BLANK:
            os << "B";
            break;
        case Node::EMPTY:
            os << "E";
            break;
    }
    os << "{" << (n.empty() ? "" : n._value) << "}";
    return os;
}
}