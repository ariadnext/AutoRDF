#include "autordf/internal/cAPI.h"
#include "autordf/Factory.h"

#include "autordf/internal/World.h"
#include "autordf/Exception.h"

namespace autordf {

Factory::Factory() : Model() {}

Resource Factory::createBlankNodeResource(const std::string &bnodeid) {
    std::string id = bnodeid;
    if ( id.empty() ) {
        id = genBlankNodeId();
    }
    Resource r(NodeType::BLANK, id, this);
    return r;
}

Resource Factory::createIRIResource(const std::string &iri) {
    Resource r(NodeType::RESOURCE, iri, this);
    return r;
}

Resource Factory::createResourceFromNode(const Node& node) {
    if ( node.type() == NodeType::RESOURCE ) {
        return createIRIResource(node.iri());
    } else if ( node.type() == NodeType::BLANK ) {
        return createBlankNodeResource(node.bNodeId());
    } else {
        throw InternalError(std::string("Unable to create resource from subject type ") + nodeTypeString(node.type()));
    }
}

std::shared_ptr<Property> Factory::createProperty(const std::string &iri, NodeType type) {
    return std::shared_ptr<Property>(new Property(type, iri, this));
}

}
