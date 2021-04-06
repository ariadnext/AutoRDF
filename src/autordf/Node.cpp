#include <autordf/internal/cAPI.h>
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
#if defined(USE_REDLAND)
        _node = librdf_new_node_from_node(n._node);
#elif defined(USE_SORD)
        _node = sord_node_copy(n._node);
#endif
    } else {
        _node = nullptr;
    }
}

Node& Node::operator=(const Node& n) {
    clear();
    if ( n._node ) {
#if defined(USE_REDLAND)
        _node = librdf_new_node_from_node(n._node);
#elif defined(USE_SORD)
        _node = sord_node_copy(n._node);
#endif
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
#if defined(USE_REDLAND)
            librdf_free_node(_node);
#elif defined(USE_SORD)
            sord_node_free(internal::World().get(), _node);
#endif
        }
        _node = nullptr;
    }
    _own = true;
}

c_api_node* Node::pull() {
    c_api_node *n = _node;
    _own = false;
    clear();
    return n;
}

NodeType Node::type() const {
    if ( !_node ) {
        return NodeType::EMPTY;
    } else {
#if defined(USE_REDLAND)
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
#elif defined(USE_SORD)
        switch(sord_node_get_type(_node)) {
            case SordNodeType::SORD_URI:
                return NodeType::RESOURCE;
            case SordNodeType::SORD_LITERAL:
                return NodeType::LITERAL;
            case SordNodeType::SORD_BLANK:
                return NodeType::BLANK;
            default:
                return NodeType::EMPTY;
        }
#endif
    }
}

// Only valid if node type is resource;
const char* Node::iri() const {
    assertType("iri", NodeType::RESOURCE);
#if defined(USE_REDLAND)
    return reinterpret_cast<const char*>(librdf_uri_as_string(librdf_node_get_uri(_node)));
#elif defined(USE_SORD)
    return reinterpret_cast<const char*>(sord_node_get_string(_node));
#endif
}

// Only valid if node type is literal
const char* Node::literal() const {
    assertType("literal", NodeType::LITERAL);
#if defined(USE_REDLAND)
    return reinterpret_cast<const char*>(librdf_node_get_literal_value(_node));
#elif defined(USE_SORD)
    return reinterpret_cast<const char*>(sord_node_get_string(_node));
#endif
}

// Blank node id
const char* Node::bNodeId() const {
    assertType("bNodeId", NodeType::BLANK);
#if defined(USE_REDLAND)
    return reinterpret_cast<const char*>(librdf_node_get_blank_identifier(_node));
#elif defined(USE_SORD)
    return reinterpret_cast<const char*>(sord_node_get_string(_node));
#endif
}

const char* Node::dataType() const {
    assertType("literal", NodeType::LITERAL);
#if defined(USE_REDLAND)
    librdf_uri *dataTypeUri = librdf_node_get_literal_value_datatype_uri(_node);
    return reinterpret_cast<const char*>(dataTypeUri ? librdf_uri_as_string(dataTypeUri) : nullptr);
#elif defined(USE_SORD)
    SordNode *dataTypeUri = sord_node_get_datatype(_node);
    return reinterpret_cast<const char*>(dataTypeUri ? sord_node_get_string(dataTypeUri) : nullptr);
#endif
}

const char* Node::lang() const {
    assertType("literal", NodeType::LITERAL);
#if defined(USE_REDLAND)
    return reinterpret_cast<const char*>(librdf_node_get_literal_value_language(_node));
#elif defined(USE_SORD)
    return sord_node_get_language(_node);
#endif
}

/**
 * Set node type to Resource, and set IRI as value
 */
Node& Node::setIri(const std::string& iri) {
    clear();
#if defined(USE_REDLAND)
    _node = librdf_new_node_from_uri_string(internal::World().get(), reinterpret_cast<const unsigned char*>(iri.c_str()));
#elif defined(USE_SORD)
    _node = sord_new_uri(internal::World().get(), reinterpret_cast<const unsigned char*>(iri.c_str()));
#endif
    if (!_node) {
        throw InternalError("Failed to construct node from URI");
    }
    return *this;
}

/**
 * Set node type to Literal, and set literal as value
 */
Node& Node::setLiteral(const std::string& literal, const std::string& lang, const std::string& dataTypeUri) {
    internal::World w;
#if defined(USE_REDLAND)
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
#elif defined(USE_SORD)
    SordNode *dataType = nullptr;
    if ( dataTypeUri.length() ) {
        dataType = sord_new_uri(w.get(), reinterpret_cast<const unsigned char*>(dataTypeUri.c_str()));
    }
    _node = sord_new_literal(w.get(), dataType, reinterpret_cast<const unsigned char*>(literal.c_str()), (lang.length() ? lang.c_str() : nullptr));
#endif
    if (!_node) {
        throw InternalError(std::string("Failed to construct node from literal: ") + literal);
    }
    return *this;
}

/**
 * Set type Blank Node, and set bnodeid as value
 */
Node& Node::setBNodeId(const std::string& bnodeid) {
#if defined(USE_REDLAND)
    _node = librdf_new_node_from_blank_identifier(internal::World().get(),
                                                     reinterpret_cast<const unsigned char*>(bnodeid.c_str()));
#elif defined(USE_SORD)
    _node = sord_new_blank(internal::World().get(),
                           reinterpret_cast<const unsigned char*>(bnodeid.c_str()));
#endif
    if (!_node) {
        throw InternalError(std::string("Failed to construct node from blank identifier: ") +
                                    bnodeid);
    }
    return *this;
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
        case NodeType::BLANK:
            os << "{\"" << n.bNodeId() << "\"";
            break;
        default:
            os << "{";
            break;
    }

    os << "}";
    return os;
}

}
