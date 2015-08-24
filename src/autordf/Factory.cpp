#include "autordf/Factory.h"

#include <librdf.h>

#include <stdexcept>

#include "autordf/World.h"
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

Property Factory::createProperty(const std::string &iri, NodeType type) {
    Property p(type, iri, this);
    return p;
}

ResourceList Factory::findByType(const std::string& typeIri) {
    Statement req;
    req.predicate.setIri("http://www.w3.org/1999/02/22-rdf-syntax-ns#type");
    req.object.setIri(typeIri);
    return ResourceList(req, this);
}
}
