#include "autordf/Factory.h"

#include <librdf.h>

#include "autordf/internal/World.h"
#include "autordf/Exception.h"

namespace autordf {

Resource Factory::createBlankNodeResource(const std::string &bnodeid) {
    std::string id = bnodeid;
    if ( id.empty() ) {
        std::shared_ptr<librdf_node> lrdfnode(
                librdf_new_node_from_blank_identifier(_world->get(), nullptr),
                librdf_free_node);
        if ( !lrdfnode ) {
            throw InternalError("Unable to allocate blank node identifier");
        }
        id = reinterpret_cast<const char *>(librdf_node_get_blank_identifier(lrdfnode.get()));
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
