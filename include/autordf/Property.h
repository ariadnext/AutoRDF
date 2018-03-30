#ifndef AUTORDF_PROPERTY_H
#define AUTORDF_PROPERTY_H

#include <string>

#include <autordf/NodeType.h>
#include <autordf/PropertyValue.h>

#include <autordf/autordf_export.h>

namespace autordf {

class Factory;
class Resource;

/**
 * Stores a Property of a Web Semantic Resource
 *
 * Contains a type, an IRI, and a value.
 */
class Property {
public:
    /**
     * Creates a new property of emtpy type
     */
    Property() { setType(NodeType::EMPTY); }

    /**
     * Return node type for this property
     */
    NodeType type() const { return _type; }

    /**
     * Return if property is a literal
     */
    bool isLiteral() const { return _type == NodeType::LITERAL; };

    /**
     * Return if property is resource compatible (either a blank node or ressource node)
     */
    bool isResource() const { return _type == NodeType::BLANK || _type == NodeType::RESOURCE; };

    /**
     * Returns property IRI
     */
    const std::string& iri() const { return _iri; }

    /**
     * Returns property value
     */
    const PropertyValue& value() const { return _value; }

    /**
     * Sets value to value. If setLiteralType is set, Property is type is set to literal value
     */
    AUTORDF_EXPORT Property& setValue(const PropertyValue& value, bool setLiteralType = true);

    /**
     * Sets the value to given resource IRI or resource bnodeid
     */
    AUTORDF_EXPORT Property& setValue(const Resource& res);

    /**
     * Convert to Resource
     * @throw InvalidNodeType if Node is not Resource or Blank node
     */
    AUTORDF_EXPORT Resource asResource() const;

private:
    NodeType _type;
    std::string _iri;
    PropertyValue _value;

    Factory *_factory;

    // Should only be built through Factory
    Property(NodeType t, const std::string& iri, Factory *f) : _iri(iri), _factory(f) { setType(t); }

    void setType(NodeType t);

    friend class Factory;
};

std::ostream& operator<<(std::ostream& os, const Property&);
}

#endif //AUTORDF_PROPERTY_H
