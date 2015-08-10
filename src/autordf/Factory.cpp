#include "autordf/Factory.h"

namespace autordf {

Resource Factory::createBlankResource(const std::string& bnodeid) {
    Resource r(NodeType::BLANK, bnodeid);
    r._factory = this;
    return r;
}

Resource Factory::createIRIResource(const std::string &iri) {
    Resource r(NodeType::RESOURCE, iri);
    r._factory = this;
    return r;
}

Property Factory::createLiteralProperty(const std::string &iri) {
    Property p(NodeType::LITERAL, iri);
    p._factory = this;
    return p;
}

Property Factory::createResourceProperty(const std::string &iri) {
    Property p(NodeType::RESOURCE, iri);
    p._factory = this;
    return p;
}

Property Factory::createBlankNodeProperty(const std::string &iri) {
    Property p(NodeType::BLANK, iri);
    p._factory = this;
    return p;
}

ResourceList Factory::findByType(const std::string& typeIri) {
    Statement req;
    req.predicate.setIri("http://www.w3.org/1999/02/22-rdf-syntax-ns#type");
    req.object.setIri(typeIri);
    return ResourceList(req, this);
}
}
