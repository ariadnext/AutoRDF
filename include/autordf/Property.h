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
    Property() { setType(NodeType::EMPTY); }

    NodeType type() const { return _type; }
    const std::string& iri() const { return _iri; }
    const PropertyValue& value() const { return _value; }
    Property& setValue(const PropertyValue& value, bool setLiteralType = true);
    Property& setValue(const Resource& res);

    /**
     * Convert to Resource
     * @throw NotAResourceException if Node is not Resource or Blank node
     */
    Resource asResource() const;

    class DuplicateException;
    class NotFoundException;
    class NotAResourceException;

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
