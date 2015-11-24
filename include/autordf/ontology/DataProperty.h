#ifndef AUTORDF_ONTOLOGY_DATAPROPERTY_H
#define AUTORDF_ONTOLOGY_DATAPROPERTY_H

#include <map>
#include <memory>

#include "Property.h"

namespace autordf {
namespace ontology {

/**
 * Models a Data Property as seen by Semantic Web
 */
class DataProperty : public Property {
public:
    using Property::Property;
};

}
}

#endif //AUTORDF_ONTOLOGY_DATAPROPERTY_H
