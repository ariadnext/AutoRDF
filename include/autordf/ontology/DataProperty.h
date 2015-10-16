#ifndef AUTORDF_ONTOLOGY_DATAPROPERTY_H
#define AUTORDF_ONTOLOGY_DATAPROPERTY_H

#include <map>
#include <memory>

#include "Property.h"

namespace autordf {
namespace ontology {

class DataProperty : public Property {
public:
    // iri to Property map
    static std::map<std::string, std::shared_ptr<DataProperty> > uri2Ptr;

    std::string getEffectiveRange(const Klass& kls) const;
};

}
}

#endif //AUTORDF_ONTOLOGY_DATAPROPERTY_H
