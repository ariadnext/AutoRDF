#include <autordf/Property.h>

#include <stdexcept>

#include "autordf/Factory.h"
#include "autordf/Resource.h"
#include "autordf/Exception.h"

namespace autordf {

void Property::setType(NodeType t) {
    _type  = t;
}

Resource Property::asResource() const {
    if ( _type == NodeType::RESOURCE ) {
        return _factory->createIRIResource(_value);
    } else if ( _type == NodeType::BLANK ) {
        return _factory->createBlankNodeResource(_value);
    } else {
        throw InvalidNodeType("Unable to convert Property " + _iri + " as resource");
    }
}

Property& Property::setValue(const Resource &res) {
    setType(res.type());
    _value = PropertyValue(res.name(), datatype::DATATYPE_STRING);
    return *this;
}

Property& Property::setValue(const PropertyValue& val, bool setLiteralType) {
    if ( setLiteralType ) {
        setType(NodeType::LITERAL);
    }
    _value = val;
    return *this;
}

std::ostream& operator<<(std::ostream& os, const Property& p) {
    switch(p.type()) {
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
    os << "{" << p.iri() << ", " << p.value() << "}";
    return os;
}
}
