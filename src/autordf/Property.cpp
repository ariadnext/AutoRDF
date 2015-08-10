#include <autordf/Property.h>

#include <stdexcept>

#include <autordf/Factory.h>
#include <autordf/Resource.h>

namespace autordf {

void Property::setType(NodeType t) {
    _type  = t;
}

Resource Property::asResource() const {
    if ( _type == NodeType::RESOURCE ) {
        return _factory->createIRIResource(_value);
    } else if ( _type == NodeType::BLANK ) {
        return _factory->createBlankResource(_value);
    } else {
        throw std::runtime_error("Unable to convert Property " + _iri + " as resource");
    }
}

Property& Property::setValue(const Resource &res) {
    setType(res.type());
    _value = res.name();
    return *this;
}

Property& Property::setValue(const std::string &val, bool setLiteralType) {
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
