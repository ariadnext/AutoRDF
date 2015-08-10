#ifndef AUTORDF_NODE_H
#define AUTORDF_NODE_H

#include <string>
#include <ostream>

#include <autordf/NodeType.h>

namespace autordf {

class Node {
public:
    NodeType type;

    Node() : type(NodeType::EMPTY) {}

    // Only valid if node type is resource;
    const std::string& iri() const;
    // Only valid if node type is literal
    const std::string& literal() const;
    // Only valid if node blank
    const std::string&bNodeId() const;

    // Set type type Resource, and set IRI as value
    void setIri(const std::string& value) { type = NodeType::RESOURCE; _value = value; }

    // Set type type Literal, and set Literal as value
    void setLiteral(const std::string& value) { type = NodeType::LITERAL; _value = value; }

    // Set type type Blank Node, and set node id as value
    void setNodeId(const std::string& value) { type = NodeType::BLANK; _value = value; }

    // Are we empty ?
    bool empty() const { return type == NodeType::EMPTY; }

    // Empties the node
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
