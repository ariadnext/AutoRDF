#ifndef AUTORDF_NODE_H
#define AUTORDF_NODE_H

#include <string>
#include <iosfwd>

#include <autordf/NodeType.h>

namespace autordf {
namespace internal {
class NodeConverter;
}

/**
 * A node is part of a triplet
 * It has a type and a value
 */
class Node {
public:
    /**
     * This node type
     */
    NodeType type() const { return _type; }

    /**
    * This node type
    */
    void setType(NodeType type) { _type = type; }

    /**
     * Builds an empty node
     */
    Node() : _type(NodeType::EMPTY) {}

    /**
     * @return Internationalized Resource Identifier
     *
     * Only valid if node type is Resource
     * @throw InvalidNodeType if not of type Resource
     */
    const std::string& iri() const;

    /**
     * @return literal value
     *
     * Only valid if node type is Literal
     * @throw InvalidNodeType if not of type Literal
     */
    const std::string& literal() const;

    /**
     * @return blank node identifier
     *
     * Only valid if node type is blank
     * @throw InvalidNodeType if not of type Literal
     */
    const std::string& bNodeId() const;

    /**
     * @return the literal data type if it is set, blank string otherwise
     *
     * Only valid if node type is Literal
     * @throw InvalidNodeType if not of type Literal
     */
    const std::string& dataType() const;

    /**
     * @return the literal lang if it is set, blank string otherwise
     *
     * @see http://www.ietf.org/rfc/rfc4646.txt
     *
     * Only valid if node type is Literal
     * @throw InvalidNodeType if not of type Literal
     */
    const std::string& lang() const;

    /**
     * Set node type to Resource, and set IRI as value
     */
    void setIri(const std::string& iri) { _type = NodeType::RESOURCE; _value = iri; }

    /**
     * Set node type to Literal, and set literal as value
     */
    void setLiteral(const std::string& literal) { _type = NodeType::LITERAL; _value = literal; }

    /**
     * Set type type Blank Node, and set bnodeid as value
     */
    void setBNodeId(const std::string& bnodeid) { _type = NodeType::BLANK; _value = bnodeid; }

    /**
     * Set Literal data type
     * Only one of data type or lang can be set
     */
    void setDataType(const std::string& dataType);

    /**
     * Set literal language.
     * Only one of data type or lang can be set
     * @see http://www.ietf.org/rfc/rfc4646.txt
     */
    void setLang(const std::string& lang);

    /**
     * @return true if node is of empty type
     */
    bool empty() const { return _type == NodeType::EMPTY; }

    /**
     * Empties the node
     *
     * Sets node type to Empty, and clears content
     */
    void clear() { _type = NodeType::EMPTY; _value.clear(); }

private:
    /**
     * This node type
     */
    NodeType _type;

    std::string _value;

    // Only for literal nodes: optional language tag
    std::string _lang;
    // Only for literal nodes: optional data type
    std::string _dataType;

    // Makes sure we are right type when accessing dedicated property
    void assertType(const char* prop, NodeType t) const;

    friend class internal::NodeConverter;
    friend std::ostream& operator<<(std::ostream& os, const Node&);
};

std::ostream& operator<<(std::ostream& os, const Node&);

}

#endif //AUTORDF_NODE_H
