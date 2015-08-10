#include <autordf/Property.h>

#include <stdexcept>

#include <autordf/Factory.h>

namespace autordf {

void Property::setType(NodeType t) {
    if ( t == NodeType::RESOURCE || t == NodeType::LITERAL ||t == NodeType::EMPTY  ) {
        _type  = t;
    } else {
        throw std::runtime_error(std::string("Node type ") + nodeTypeString(t) + " is not allowed for Property");
    }
}

}
