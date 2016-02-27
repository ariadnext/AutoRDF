#include <autordf/Node.h>

#include <stdexcept>
#include <ostream>

#include "autordf/Exception.h"

namespace autordf {

void Node::assertType(const char* prop, NodeType t) const {
    if ( _type != t ) {
        throw InvalidNodeType(std::string("Called Node::") + prop + "() on resource type " + nodeTypeString(_type));
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

const std::string& Node::dataType() const {
    assertType("literal", NodeType::LITERAL);
    return _dataType;
}

const std::string& Node::lang() const {
    assertType("literal", NodeType::LITERAL);
    return _lang;
}

void Node::setDataType(const std::string& dataType) {
    assertType("literal", NodeType::LITERAL);
    if ( !_lang.empty() && !dataType.empty() ) {
        throw CantSetLiteralTypeAndLang("Can't set dataType on node as lang is already set");
    }
    _dataType = dataType;
}

void Node::setLang(const std::string& lang) {
    assertType("literal", NodeType::LITERAL);
    if ( !_dataType.empty() && !lang.empty() ) {
        throw CantSetLiteralTypeAndLang("Can't set lang on node as dataType is already set");
    }
    _lang = lang;
}

std::ostream& operator<<(std::ostream& os, const Node& n) {
    switch(n.type()) {
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
    os << "{\"" << (n.empty() ? "" : n._value) << "\"";
    if ( n.type() == NodeType::LITERAL ) {
        if ( !n.dataType().empty() ) {
            os << "^^" << n.dataType();
        }
        if ( !n.lang().empty() ) {
            os << "@" << n.lang();
        }
    }
    os << "}";
    return os;
}

}