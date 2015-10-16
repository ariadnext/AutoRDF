#include "autordf/ontology/DataProperty.h"

#include "autordf/ontology/Klass.h"

namespace autordf {
namespace ontology {

std::map <std::string, std::shared_ptr<DataProperty>> DataProperty::uri2Ptr;

std::string DataProperty::getEffectiveRange(const Klass& kls) const {
    auto it = kls.overridenRange.find(rdfname());
    if ( it != kls.overridenRange.end() ) {
        return  it->second;
    }
    return range;
}

}
}
