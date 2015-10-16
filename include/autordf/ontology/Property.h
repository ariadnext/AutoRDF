#ifndef AUTORDF_ONTOLOGY_PROPERTY_H
#define AUTORDF_ONTOLOGY_PROPERTY_H

#include <list>
#include <string>
#include <map>

#include "RdfsEntity.h"

namespace autordf {
namespace ontology {

class Klass;

class Property : public RdfsEntity {
public:
    std::list<std::string> domains;
    std::string range;
    unsigned int minCardinality;
    unsigned int maxCardinality;

    // Qualified Cardinality restrictions also allow to specify a range for this instance properties
    std::map<std::string, std::string> overridenRange;

    //FIXME: this behaviour should be checked against the standard
    unsigned int getEffectiveMinCardinality(const Klass& kls) const;

    //FIXME: this behaviour should be checked against the standard
    unsigned int getEffectiveMaxCardinality(const Klass& kls) const;
};

}
}

#endif //AUTORDF_ONTOLOGY_PROPERTY_H
