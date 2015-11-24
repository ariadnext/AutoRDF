#include "autordf/ontology/ObjectProperty.h"

#include "autordf/ontology/Ontology.h"

namespace autordf {
namespace ontology {

// Returns class for this property, or nullptr if no class is registered
std::shared_ptr<Klass> ObjectProperty::findClass(const Klass* kls) const {
    auto map = _ontology->classUri2Ptr();
    auto kit = map.find(range(kls));
    if ( kit != map.end() ) {
        return kit->second;
    }
    return nullptr;
}

}
}