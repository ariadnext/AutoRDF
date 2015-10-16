#include "autordf/ontology/ObjectProperty.h"

#include "autordf/ontology/Klass.h"

namespace autordf {
namespace ontology {

std::map<std::string, std::shared_ptr<ObjectProperty> > ObjectProperty::uri2Ptr;

// Returns class for this property, or nullptr if no class is registered
std::shared_ptr<Klass> ObjectProperty::findClass(const Klass* kls) const {
    auto kit = Klass::uri2Ptr.find(range(kls));
    if ( kit != Klass::uri2Ptr.end() ) {
        return kit->second;
    }
    return nullptr;
}

}
}