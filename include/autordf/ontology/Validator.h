#ifndef AUTORDF_ONTOLOGY_VALIDATOR_H
#define AUTORDF_ONTOLOGY_VALIDATOR_H

#include <memory>
#include <vector>
#include <autordf/ontology/Ontology.h>
#include <autordf/Model.h>
#include <autordf/Object.h>

namespace autordf {
namespace ontology {

/**
 * Provides methods to check a modatasetdel is compatible with a given ontology
 */
class Validator {
public:
    Validator(const Ontology& ontology) : _ontology(ontology) {}

    /**
     * Each error in given dataset is reported in an Error structure
     */
    class Error {
    public:
        Error() {}

        //TODO: X macroize that
        enum Type {
            INVALIDTYPE,    // rdf type for this property for is not allowed - subject, property are filled
            NOTENOUHVALUES, // This property has not enough values - subject, property, val are filled
            TOOMANYVALUES   // This property has too many values - subject, property, count are filled
        };

        /**
         * The object the error applies to
         */
        Object subject;

        /**
         * The property in the given object the error applies to
         */
        Uri    property;

        /**
         * Message @subject and @property, @count are used as placeholder subject and property
         */
        std::string message;

        /**
         * Message,with placeholders replaced with their actual values
         */
        std::string fullMessage();
    };

    /**
     * Checks all model resources are compatible with ontology
     */
    std::shared_ptr<std::vector<Error>> validateModel(const Model& model);

    /**
     * Checks this object is compatible with ontology,
     * that means all its properties are compatible with the ontology
     */
    std::shared_ptr<std::vector<Error>> validateObject(const Object& object);

private:
    Ontology _ontology;
};

}
}

#endif //AUTORDF_ONTOLOGY_VALIDATOR_H
