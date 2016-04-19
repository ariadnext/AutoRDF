#include "autordf/ontology/Property.h"

#include "autordf/ontology/Klass.h"

namespace autordf {
namespace ontology {

unsigned int Property::minCardinality(const Klass& kls) const {
    auto const& m = kls.overridenMinCardinality();
    auto it = m.find(rdfname());
    if ( it != m.end() ) {
        return it->second;
    }
    return _minCardinality;
}

unsigned int Property::maxCardinality(const Klass& kls) const {
    auto const& m = kls.overridenMaxCardinality();
    auto it = m.find(rdfname());
    if ( it != m.end() ) {
        return it->second;
    }
    return _maxCardinality;
}

Uri Property::range(const Klass* kls) const {
    if ( kls ) {
        auto const& m = kls->overridenRange();
        auto it = m.find(rdfname());
        if ( it != m.end() ) {
            return it->second;
        }
    }
    return _range;
}
}
}
