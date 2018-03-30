#ifndef AUTORDF_ONTOLOGY_OBJECTPROPERTY_H
#define AUTORDF_ONTOLOGY_OBJECTPROPERTY_H

#include <memory>
#include <string>
#include <map>
#include <iosfwd>

#include "Property.h"
#include <autordf/ontology/autordf-ontology_export.h>

namespace autordf {
namespace ontology {

/**
 * Models an Object Property as seen by Semantic Web
 */
class ObjectProperty : public Property {
public:
    using Property::Property;
    /**
     * Returns class for this property when found in kls object, or nullptr if no class is registered
     * @param kls class this objectproperty is supposed to be instanciated in.
     * If nullptr, qualified cardinality restrictions will be ignored
     */
    AUTORDF_ONTOLOGY_EXPORT std::shared_ptr<Klass> findClass(const Klass* kls = nullptr) const;
};
}
}
#endif //AUTORDF_ONTOLOGY_OBJECTPROPERTY_H
