#ifndef AUTORDF_FACTORY_H
#define AUTORDF_FACTORY_H

#include <string>
#include <memory>

#include <autordf/Model.h>
#include <autordf/Property.h>
#include <autordf/Resource.h>
#include <autordf/autordf_export.h>

namespace autordf {

/**
 * Factory is used to create Resources and Properties is the current Model
 */
class Factory : public Model {
public:
    using Model::Model;

    Factory();

    /**
     * Create blank (anonymous) resource
     */
    Resource AUTORDF_EXPORT createBlankNodeResource(const std::string &bnodeid = "");

    /**
     * Created named resource (with IRI)
     */
    Resource AUTORDF_EXPORT createIRIResource(const std::string& iri);

    /**
     * Converts a statement to resource
     */
    Resource AUTORDF_EXPORT createResourceFromNode(const Node& node);

    /**
     * Creates a property of type type
     * If not type is given, defaults to EMPTY
     */
    std::shared_ptr<Property> AUTORDF_EXPORT createProperty(const std::string& iri, NodeType type = NodeType::EMPTY);
};

}

#endif //AUTORDF_FACTORY_H
