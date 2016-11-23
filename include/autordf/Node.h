#ifndef AUTORDF_NODE_H
#define AUTORDF_NODE_H

#include <string>
#include <iosfwd>

#include <autordf/NodeType.h>

#ifdef LIBRDF_IN_SUBDIRS
#include <librdf/librdf.h>
#else
#include <librdf.h>
#endif

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
    NodeType type() const;

    /**
     * Builds an empty node
     */
    Node() : _node(nullptr), _own(true) {}

    /**
     * Builds a node from librdf
     */
    Node(librdf_node *node, bool own = true) : _node(node), _own(own) {}

    /**
     * Copy constructor
     */
    Node(const Node& n);

    /**
     * Copy constructor
     */
    Node(Node&& n);

    /**
     * Destructor
     */
    ~Node();

    /**
     * @return Internationalized Resource Identifier
     *
     * Only valid if node type is Resource
     * @throw InvalidNodeType if not of type Resource
     */
    const char* iri() const;

    /**
     * @return literal value
     *
     * Only valid if node type is Literal
     * @throw InvalidNodeType if not of type Literal
     */
    const char* literal() const;

    /**
     * @return blank node identifier
     *
     * Only valid if node type is blank
     * @throw InvalidNodeType if not of type Literal
     */
    const char* bNodeId() const;

    /**
     * @return the literal data type if it is set, blank string otherwise
     *
     * Only valid if node type is Literal
     * @throw InvalidNodeType if not of type Literal
     */
    const char* dataType() const;

    /**
     * @return the literal lang if it is set, blank string otherwise
     *
     * @see http://www.ietf.org/rfc/rfc4646.txt
     *
     * Only valid if node type is Literal
     * @throw InvalidNodeType if not of type Literal
     */
    const char* lang() const;

    /**
     * Set node type to Resource, and set IRI as value
     */
    Node& setIri(const std::string& iri);

    /**
     * Set node type to Literal, and set literal as value
     *
     * Empty lang or dataTypeUri strings are converted to
     */
    Node& setLiteral(const std::string& literal, const std::string& lang = "", const std::string& dataTypeUri = "");

    /**
     * Set type type Blank Node, and set bnodeid as value
     */
    Node& setBNodeId(const std::string& bnodeid);

    /**
     * Assigment operator
     */
    Node& operator=(const Node& n);

    /**
     * @return true if node is of empty type
     */
    bool empty() const { return _node == nullptr; }

    /**
     * Empties the node
     *
     * Sets node type to Empty, and clears content
     */
    void clear();

    /**
     * Internal
     */
    librdf_node* get() const { return _node; }

    /**
     * Internal
     * Extracts the node pointer.
     * It it the caller reponsibbility to free it
     */
    librdf_node* pull();

private:
    librdf_node *_node;
    // Do we need to free inside node before to destroy ourselves ?
    bool _own;

    // Makes sure we are right type when accessing dedicated property
    void assertType(const char* prop, NodeType t) const;

    friend std::ostream& operator<<(std::ostream& os, const Node&);
};

std::ostream& operator<<(std::ostream& os, const Node&);

}

#endif //AUTORDF_NODE_H
