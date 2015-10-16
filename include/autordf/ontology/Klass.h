#ifndef AUTORDF_ONTOLOGY_KLASS_H
#define AUTORDF_ONTOLOGY_KLASS_H

#include <set>
#include <map>
#include <memory>
#include <string>

#include "RdfsEntity.h"
#include "DataProperty.h"
#include "ObjectProperty.h"

namespace autordf {
namespace ontology {

class Klass : public RdfsEntity {
public:
    std::set <std::string> directAncestors;
    std::set <RdfsEntity> enumValues;

    std::set <std::shared_ptr<DataProperty>> dataProperties;
    std::set <std::shared_ptr<ObjectProperty>> objectProperties;

    // It is usual (but optional) to specify at class level the minimum and/or maximum instances for a property
    // This is done using cardiniality restrictions
    std::map<std::string, unsigned int> overridenMinCardinality;
    std::map<std::string, unsigned int> overridenMaxCardinality;

    /**
     * Take into account Qualified Cardinality restrictions, that allow to owerride the
     * the data type of this property for a specific class of object
     */
    std::map <std::string, std::string> overridenRange;

    std::set <std::shared_ptr<const Klass>> getAllAncestors() const;

    // iri to Klass map
    static std::map <std::string, std::shared_ptr<Klass>> uri2Ptr;
};

}
}
#endif //AUTORDF_ONTOLOGY_KLASS_H
