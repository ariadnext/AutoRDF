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
    Validator(const std::shared_ptr<Ontology>& ontology) : _ontology(ontology) {}
    /**
     * Each error in given dataset is reported in an Error structure
     */
    class Error {
    public:
        Error() {}
        Error(const Object& subj, const Uri& prop): subject(subj), property(prop) {}

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
         * Message @subject and @property, @count, @val, @range are used as placeholder subject and property
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
         * store any integer value
         */
        int val;

        /**
         * store iri
         */
        Uri range;
        /**
         * Message,with placeholders replaced with their actual values
         */
        std::string fullMessage() const;
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
    std::shared_ptr<Ontology> _ontology;

    /**
     * @brief validateDataProperty
     * @param object autordf::Object to validate
     * @param currentCLass Rdf Class to get all data property from
     * @param errorList list of errors
     * Fill the given error list with OWL errors found on the object Dataproperty
     */
    void validateDataProperty(const Object& object, const std::shared_ptr<const Klass>& currentCLass, std::vector<Validator::Error>*);

    /**
     * @brief validateObjectProperty
     * @param object autordf::Object to validate
     * @param currentCLass Rdf Class to get all data property from
     * @param errorList list of errors
     * Fill the given error list with OWL errors found on the object's objectProperties
     */
    void validateObjectProperty(const Object& object, const std::shared_ptr<const Klass>&, std::vector<Validator::Error>*);

    /**
     * @brief isDataTypeValid
     * @param property PropertyValue to check type from
     * @param rdfType expectd Rdf type for the property value
     * return true if the property value has the correct type, false otherwise
     */
    static bool isDataTypeValid(const autordf::PropertyValue& property, const autordf::cvt::RdfTypeEnum& rdfType);

    /**
     * @brief isObjectTypeValid
     * @param object the object to check type
     * @param type expected type of the object
     * return true if the given object is of the given type or type's subclass, false otherwise
     */
    bool isObjectTypeValid(const autordf::Object& object, const autordf::Uri& type);

    /**
     * Compute helper string to get location string for a given object
     */
    static std::string nodeTextLocation(const autordf::Object& obj, int maxRecurse = 5);
};

}
}

#endif //AUTORDF_ONTOLOGY_VALIDATOR_H
