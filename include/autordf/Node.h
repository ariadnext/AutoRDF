#ifndef AUTORDF_NODE_H
#define AUTORDF_NODE_H

#include <string>
#include <iosfwd>

#include <autordf/NodeType.h>

namespace autordf {

/**
 * A node is part of a triplet
 * It has a type and a value
 */
class Node {
public:
    /**
     * This node type
     */
    NodeType type;

    /**
     * Builds an empty node
     */
    Node() : type(NodeType::EMPTY) {}

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
     * Set node type to Resource, and set IRI as value
     */
    void setIri(const std::string& iri) { type = NodeType::RESOURCE; _value = iri; }

    /**
     * Set node type to Literal, and set literal as value
     */
    void setLiteral(const std::string& literal) { type = NodeType::LITERAL; _value = literal; }

    /**
     * Set type type Blank Node, and set bnodeid as value
     */
    void setBNodeId(const std::string &bnodeid) { type = NodeType::BLANK; _value = bnodeid; }

    /**
     * @return true if node is of empty type
     */
    bool empty() const { return type == NodeType::EMPTY; }

    /**
     * Empties the node
     *
     * Sets node type to Empty, and clears content
     */
    void clear() { type = NodeType::EMPTY; _value.clear(); }

private:
    std::string _value;

    // Makes sure we are right type when accessing dedicated property
    void assertType(const char* prop, NodeType t) const;

    friend class StatementConverter;
    friend std::ostream& operator<<(std::ostream& os, const Node&);
};

std::ostream& operator<<(std::ostream& os, const Node&);

}

#endif //AUTORDF_NODE_H
