#ifndef AUTORDF_RESOURCE_H
#define AUTORDF_RESOURCE_H

#include <string>
#include <autordf/NodeType.h>

namespace autordf {

class Factory;
class Resource;

class Property {
public:
    Property() { setType(NodeType::EMPTY); }

    NodeType type() const { return _type; }
    const std::string& iri() const { return _iri; }
    const std::string& value() const { return _value; }
    void setValue(const std::string& value, bool setLiteralType = true);
    void setValue(const Resource& res);

    // Converters
    template<typename T> T as() const;

    // Convert to Resource
    // Only valid if Type is Resource or Blank Node
    // @throw NotAResourceException if not the case
    Resource asResource() const;

    class DuplicateException;
    class NotFoundException;
    class NotAResourceException;

private:
    NodeType _type;
    std::string _iri;
    std::string _value;

    Factory *_factory;

    // Should only be built through Factory
    Property(NodeType t, const std::string& iri, Factory *f) : _iri(iri), _factory(f) { setType(t); }

    void setType(NodeType t);

    friend class Factory;
};

std::ostream& operator<<(std::ostream& os, const Property&);
}

#endif //AUTORDF_RESOURCE_H
