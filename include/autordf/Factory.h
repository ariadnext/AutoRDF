#ifndef AUTORDF_FACTORY_H
#define AUTORDF_FACTORY_H

#include <string>

#include <autordf/Model.h>
#include <autordf/Property.h>
#include <autordf/Resource.h>

namespace autordf {

class Factory : public Model {
public:
    /**
     * Create blank (anonymous) resource
     */
    Resource createBlankResource();

    /**
     * Created named resource (with IRI)
     */
    Resource createIRIResource(const std::string& iri);

    /**
     * Creates a property of type Literal
     */
    Property createLiteralProperty(const std::string& iri);

    /**
     * Creates a property of type Resource
     */
    Property createResourceProperty(const std::string& iri);
};

}

#endif //AUTORDF_FACTORY_H
