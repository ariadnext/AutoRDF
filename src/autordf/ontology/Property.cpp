#include "autordf/ontology/Property.h"

#include "autordf/ontology/Klass.h"

namespace autordf {
namespace ontology {

unsigned int Property::minCardinality(const Klass& kls) const {
    auto it = kls.overridenMinCardinality.find(rdfname());
    if ( it != kls.overridenMinCardinality.end() ) {
        return  it->second;
    }
    return _minCardinality;
}

unsigned int Property::maxCardinality(const Klass& kls) const {
    auto it = kls.overridenMaxCardinality.find(rdfname());
    if ( it != kls.overridenMaxCardinality.end() ) {
        return  it->second;
    }
    return _maxCardinality;
}

std::string Property::range(const Klass* kls) const {
    if ( kls ) {
        auto it = kls->overridenRange.find(rdfname());
        if ( it != kls->overridenRange.end() ) {
            return  it->second;
        }
    }
    return _range;
}
}
}
