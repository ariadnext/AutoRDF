#ifndef AUTORDF_PROPERTY_H
#define AUTORDF_PROPERTY_H

#include <string>

#include <autordf/NodeType.h>
#include <autordf/PropertyValue.h>

namespace autordf {

class Factory;
class Resource;

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
    Property& setValue(const PropertyValue& value, bool setLiteralType = true);

    /**
     * Sets the value to given resource IRI or resource bnodeid
     */
    Property& setValue(const Resource& res);

    /**
     * Convert to Resource
     * @throw InvalidNodeType if Node is not Resource or Blank node
     */
    Resource asResource() const;

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
