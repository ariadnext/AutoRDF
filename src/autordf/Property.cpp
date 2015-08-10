#include <autordf/Property.h>

#include <stdexcept>

#include <autordf/Factory.h>

namespace autordf {

void Property::setType(NodeType t) {
    _type  = t;
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

Resource Property::asResource() const {
    /*if ( _type == NodeType::RESOURCE ) {
        return _factory->
    } else {
        throw std::runtime_error("Property " + _iri + " is not a resource");
    }*/
   // return autordf::Resource();
}
}
