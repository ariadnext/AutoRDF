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

        #define ERROR_TYPE(X) \
            X(INVALIDDATATYPE,   "Rdf type of dataproperty is not allowed. subject, property are filled") \
            X(INVALIDTYPE,       "Rdf type of object is not allowed. subject, property are filled") \
            X(NOTENOUHVALUES,    "This property does not have enough values - subject, property, count and expected value are filled") \
            X(TOOMANYVALUES,     "This property has too many values. subject, property, count and expected value are filled")

        enum Type {
        #define X(a, b) a,
            ERROR_TYPE(X)
        #undef X
        };
        /**
         * The object the error applies to
         */
        Object subject;

        /**
         * The property in the given object the error applies to
         */
        Uri property;

        /**
         * Message @subject and @property, @count are used as placeholder subject and property
         */
        std::string message;

        /**
         * how many values of this property we have
         */
        int count;

        /**
         * error type
         */
        Type type;

        /**
         * Message,with placeholders replaced with their actual values
         */
        std::string fullMessage();
    };

    /**
     * Checks all model resources are compatible with ontology
     * FIXME @param model is not used in this case. autordf::Object has the model
     */
    std::shared_ptr<std::vector<Error>> validateModel(const Model& model);

    /**
     * Checks this object is compatible with ontology,
     * that means all its properties are compatible with the ontology
     */
    std::shared_ptr<std::vector<Validator::Error>> validateObject(const Object& object);

private:
    Ontology _ontology;
    bool isDataTypeValid(const autordf::PropertyValue& property, const autordf::cvt::RdfTypeEnum& rdfType);
};

}
}

#endif //AUTORDF_ONTOLOGY_VALIDATOR_H
