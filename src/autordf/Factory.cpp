#include "autordf/Factory.h"

namespace autordf {

Resource Factory::createBlankResource() {
    Resource r(NodeType::BLANK);
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
}
