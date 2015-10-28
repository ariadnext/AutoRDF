#ifndef AUTORDF_ONTOLOGY_OBJECTPROPERTY_H
#define AUTORDF_ONTOLOGY_OBJECTPROPERTY_H

#include <memory>
#include <string>
#include <map>
#include <iosfwd>

#include "Property.h"

namespace autordf {
namespace ontology {

class ObjectProperty : public Property {
public:
    using Property::Property;
    /**
     * Returns class for this property when found in kls object, or nullptr if no class is registered
     * @param kls class this objectproperty is supposed to be instanciated in.
     * If nullptr, qualified cardinality restrictions will be ignored
     */
    std::shared_ptr<Klass> findClass(const Klass* kls = nullptr) const;
};
}
}
#endif //AUTORDF_ONTOLOGY_OBJECTPROPERTY_H
