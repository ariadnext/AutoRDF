#include <autordf/Node.h>

#include <stdexcept>

namespace autordf {

void Node::assertType(const char* prop, NodeType t) const {
    if ( type != t ) {
        throw std::runtime_error(std::string("Called Node::") + prop + "() on resource type " + nodeTypeString(type));
    }
}

// Only valid if node type is resource;
const std::string& Node::iri() const {
    assertType("iri", NodeType::RESOURCE);
    return _value;
}

// Only valid if node type is literal
const std::string& Node::literal() const {
    assertType("literal", NodeType::LITERAL);
    return _value;
}

// Blank node id
const std::string& Node::bNodeId() const {
    assertType("bNodeId", NodeType::BLANK);
    return _value;
}

std::ostream& operator<<(std::ostream& os, const Node& n) {
    switch(n.type) {
        case NodeType::RESOURCE:
            os << "R";
            break;
        case NodeType::LITERAL:
            os << "L";
            break;
        case NodeType::BLANK:
            os << "B";
            break;
        case NodeType::EMPTY:
            os << "E";
            break;
    }
    os << "{" << (n.empty() ? "" : n._value) << "}";
    return os;
}
}