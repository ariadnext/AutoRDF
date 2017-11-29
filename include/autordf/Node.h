#ifndef AUTORDF_NODE_H
#define AUTORDF_NODE_H

#include <string>
#include <iosfwd>

#include <autordf/cAPI.h>
#include <autordf/NodeType.h>
#include <autordf/autordf_export.h>

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
    AUTORDF_EXPORT NodeType type() const;

    /**
     * Builds an empty node
     */
    Node() : _node(nullptr), _own(true) {}

    /**
     * Builds a node from librdf
     * @param own if true, we will free c_api_node when this object is destroyed
     */
    Node(c_api_node *node, bool own = true) : _node(node), _own(own) {}

    /**
     * Copy constructor
     */
    AUTORDF_EXPORT Node(const Node& n);

    /**
     * Copy constructor
     */
    AUTORDF_EXPORT Node(Node&& n);

    /**
     * Destructor
     */
    AUTORDF_EXPORT ~Node();

    /**
     * @return Internationalized Resource Identifier
     *
     * Only valid if node type is Resource
     * @throw InvalidNodeType if not of type Resource
     */
    AUTORDF_EXPORT const char* iri() const;

    /**
     * @return literal value
     *
     * Only valid if node type is Literal
     * @throw InvalidNodeType if not of type Literal
     */
     AUTORDF_EXPORT const char* literal() const;

    /**
     * @return blank node identifier
     *
     * Only valid if node type is blank
     * @throw InvalidNodeType if not of type Literal
     */
     AUTORDF_EXPORT const char* bNodeId() const;

    /**
     * @return the literal data type if it is set, blank string otherwise
     *
     * Only valid if node type is Literal
     * @throw InvalidNodeType if not of type Literal
     */
     AUTORDF_EXPORT const char* dataType() const;

    /**
     * @return the literal lang if it is set, blank string otherwise
     *
     * @see http://www.ietf.org/rfc/rfc4646.txt
     *
     * Only valid if node type is Literal
     * @throw InvalidNodeType if not of type Literal
     */
    AUTORDF_EXPORT const char* lang() const;

    /**
     * Set node type to Resource, and set IRI as value
     */
    AUTORDF_EXPORT Node& setIri(const std::string& iri);

    /**
     * Set node type to Literal, and set literal as value
     *
     * Empty lang or dataTypeUri strings are converted to
     */
    AUTORDF_EXPORT Node& setLiteral(const std::string& literal, const std::string& lang = "", const std::string& dataTypeUri = "");

    /**
     * Set type type Blank Node, and set bnodeid as value
     */
    AUTORDF_EXPORT Node& setBNodeId(const std::string& bnodeid);

    /**
     * Assigment operator
     */
    AUTORDF_EXPORT Node& operator=(const Node& n);

    /**
     * @return true if node is of empty type
     */
    bool empty() const { return _node == nullptr; }

    /**
     * Empties the node
     *
     * Sets node type to Empty, and clears content
     */
    AUTORDF_EXPORT void clear();

    /**
     * Internal
     */
    c_api_node* get() const { return _node; }

    /**
     * Internal
     * Extracts the node pointer.
     * It it the caller reponsibbility to free it
     */
    AUTORDF_EXPORT c_api_node* pull();

private:
    c_api_node *_node;
    // Do we need to free inside node before to destroy ourselves ?
    bool _own;

    // Makes sure we are right type when accessing dedicated property
    void assertType(const char* prop, NodeType t) const;

    friend std::ostream& operator<<(std::ostream& os, const Node&);
};

std::ostream& operator<<(std::ostream& os, const Node&);

}

#endif //AUTORDF_NODE_H
