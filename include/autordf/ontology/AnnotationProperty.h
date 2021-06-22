#pragma once

#include "Property.h"

namespace autordf {
namespace ontology {

/**
 * Models an Annotation Property as seen by Semantic Web
 */
class AnnotationProperty : public Property {
public:
    using Property::Property;
};

}
}