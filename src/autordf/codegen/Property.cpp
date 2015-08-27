#include "Property.h"

#include "Klass.h"

namespace autordf {
namespace codegen {

unsigned int Property::getEffectiveMinCardinality(const Klass& kls) const {
    auto it = kls.overridenMinCardinality.find(rdfname);
    if ( it != kls.overridenMinCardinality.end() ) {
        return  it->second;
    }
    return minCardinality;
}

unsigned int Property::getEffectiveMaxCardinality(const Klass& kls) const {
    auto it = kls.overridenMaxCardinality.find(rdfname);
    if ( it != kls.overridenMaxCardinality.end() ) {
        return  it->second;
    }
    return maxCardinality;
}

}
}
