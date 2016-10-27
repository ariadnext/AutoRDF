#ifndef AUTORDF_FACTORY_H
#define AUTORDF_FACTORY_H

#include <string>
#include <memory>

#include <autordf/Model.h>
#include <autordf/Property.h>
#include <autordf/Resource.h>

namespace autordf {

/**
 * Factory is used to create Resources and Properties is the current Model
 */
class Factory : public Model {
public:
    using Model::Model;

    /**
     * Create blank (anonymous) resource
     */
    Resource createBlankNodeResource(const std::string &bnodeid = "");

    /**
     * Created named resource (with IRI)
     */
    Resource createIRIResource(const std::string& iri);

    /**
     * Converts a statement to resource
     */
    Resource createResourceFromNode(const Node& node);

    /**
     * Creates a property of type type
     * If not type is given, defaults to EMPTY
     */
    std::shared_ptr<Property> createProperty(const std::string& iri, NodeType type = NodeType::EMPTY);
};

}

#endif //AUTORDF_FACTORY_H
