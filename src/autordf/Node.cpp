#include <autordf/Node.h>

#include <stdexcept>
#include <ostream>
#include <memory>

#include "autordf/Exception.h"
#include "autordf/internal/World.h"

namespace autordf {

void Node::assertType(const char* prop, NodeType t) const {
    if ( type() != t ) {
        throw InvalidNodeType(std::string("Called Node::") + prop + "() on resource type " + nodeTypeString(type()));
    }
}

Node::Node(const Node& n) : _own(true) {
    if ( n._node ) {
        _node = librdf_new_node_from_node(n._node);
    } else {
        _node = nullptr;
    }
}

Node& Node::operator=(const Node& n) {
    clear();
    if ( n._node ) {
        _node = librdf_new_node_from_node(n._node);
    }
    return *this;
}

Node::Node(Node&& n) : _own(n._own) {
    _node = n._node;
    n._node = nullptr;
}

Node::~Node() {
    clear();
}

void Node::clear() {
    if ( _node ) {
        if ( _own ) {
            librdf_free_node(_node);
        }
        _node = nullptr;
    }
    _own = true;
}

librdf_node* Node::pull() {
    librdf_node *n = _node;
    _own = false;
    clear();
    return n;
}

NodeType Node::type() const {
    if ( !_node ) {
        return NodeType::EMPTY;
    } else {
        switch(librdf_node_get_type(_node)) {
            case LIBRDF_NODE_TYPE_RESOURCE:
                return NodeType::RESOURCE;
            case LIBRDF_NODE_TYPE_LITERAL:
                return NodeType::LITERAL;
            case LIBRDF_NODE_TYPE_BLANK:
                return NodeType::BLANK;
            default:
                return NodeType::EMPTY;
        }
    }
}

// Only valid if node type is resource;
const char* Node::iri() const {
    assertType("iri", NodeType::RESOURCE);
    return reinterpret_cast<const char*>(librdf_uri_as_string(librdf_node_get_uri(_node)));
}

// Only valid if node type is literal
const char* Node::literal() const {
    assertType("literal", NodeType::LITERAL);
    return reinterpret_cast<const char*>(librdf_node_get_literal_value(_node));
}

// Blank node id
const char* Node::bNodeId() const {
    assertType("bNodeId", NodeType::BLANK);
    return reinterpret_cast<const char*>(librdf_node_get_blank_identifier(_node));
}

const char* Node::dataType() const {
    assertType("literal", NodeType::LITERAL);
    librdf_uri *dataTypeUri = librdf_node_get_literal_value_datatype_uri(_node);
    return reinterpret_cast<const char*>(dataTypeUri ? librdf_uri_as_string(dataTypeUri) : nullptr);
}

const char* Node::lang() const {
    assertType("literal", NodeType::LITERAL);
    return reinterpret_cast<const char*>(librdf_node_get_literal_value_language(_node));
}

/**
 * Set node type to Resource, and set IRI as value
 */
void Node::setIri(const std::string& iri) {
    clear();
    _node = librdf_new_node_from_uri_string(internal::World().get(), reinterpret_cast<const unsigned char*>(iri.c_str()));
    if (!_node) {
        throw InternalError("Failed to construct node from URI");
    }
}

/**
 * Set node type to Literal, and set literal as value
 */
void Node::setLiteral(const std::string& literal, const std::string& lang, const std::string& dataTypeUri) {
    internal::World w;
    std::shared_ptr<librdf_uri> dataTypeUriPtr;
    if ( dataTypeUri.length() ) {
        dataTypeUriPtr = std::shared_ptr<librdf_uri>(librdf_new_uri(w.get(), reinterpret_cast<const unsigned char*>(dataTypeUri.c_str())),
                                                  librdf_free_uri);
        if (!dataTypeUriPtr) {
            throw InternalError(std::string("Failed to construct URI from value: ") + dataTypeUri);
        }
    }
    _node = librdf_new_node_from_typed_literal(w.get(),
                                                  reinterpret_cast<const unsigned char*>(literal.c_str()), (lang.length() ? lang.c_str() : nullptr),
                                               dataTypeUriPtr.get());
    if (!_node) {
        throw InternalError(std::string("Failed to construct node from literal: ") + literal);
    }
}

/**
 * Set type type Blank Node, and set bnodeid as value
 */
void Node::setBNodeId(const std::string& bnodeid) {
    _node = librdf_new_node_from_blank_identifier(internal::World().get(),
                                                     reinterpret_cast<const unsigned char*>(bnodeid.c_str()));
    if (!_node) {
        throw InternalError(std::string("Failed to construct node from blank identifier: ") +
                                    bnodeid);
    }
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
    switch(n.type()) {
        case NodeType::LITERAL:
            os << "{\"" << n.literal() << "\"";
            if ( n.dataType() ) {
                os << "^^" << n.dataType();
            }
            if ( n.lang() ) {
                os << "@" << n.lang();
            }
            break;
        case NodeType::RESOURCE:
            os << "{\"" << n.iri() << "\"";
            break;
        default:
            os << "{";
            break;
    }

    os << "}";
    return os;
}

}