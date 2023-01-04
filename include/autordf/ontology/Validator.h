#ifndef AUTORDF_ONTOLOGY_VALIDATOR_H
#define AUTORDF_ONTOLOGY_VALIDATOR_H

#include <memory>
#include <vector>
#include <autordf/ontology/Ontology.h>
#include <autordf/Model.h>
#include <autordf/Object.h>
#include <autordf/ontology/autordf-ontology_export.h>

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
        Error(const Object& subj, const Object& prop): subject(subj), property(prop) {}

        #define ERROR_TYPE(X) \
            X(INVALIDDATATYPE,   "Rdf type of dataproperty is not allowed. subject, property are filled") \
            X(INVALIDTYPE,       "Rdf type of object is not allowed. subject, property are filled") \
            X(NOTENOUHVALUES,    "This property does not have enough values - subject, property, count and expected value are filled") \
            X(TOOMANYVALUES,     "This property has too many values. subject, property, count and expected value are filled")          \
            X(DUPLICATEDVALUESKEY, "This key has duplicated values")  \
            X(INVALIDDOMAIN, "This property has the wrong domain")
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
        Object property;

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
        AUTORDF_ONTOLOGY_EXPORT std::string fullMessage() const;
    };

    struct ValidationOption{
        ValidationOption() : enforceObjectKeyUniqueness(false), enforceExplicitDomains(false) {}
        explicit ValidationOption(const bool & objectKeyUniqueness, const bool & explicitDomains = false) {
            enforceObjectKeyUniqueness = objectKeyUniqueness;
            enforceExplicitDomains = explicitDomains;
        }
        /**
         * Ensure that every key is unique
         */
        bool enforceObjectKeyUniqueness = false;
        /**
         * Ensure that domains are correctly set for all properties
         */
        bool enforceExplicitDomains = false;
    };

    /**
     * Checks all model resources are compatible with ontology
     * FIXME @param model is not used in this case. autordf::Object has the model
     */
    AUTORDF_ONTOLOGY_EXPORT std::shared_ptr<std::vector<Error>> validateModel(const Model& model, const ValidationOption& option = ValidationOption());

    /**
     * Checks this object is compatible with ontology,
     * that means all its properties are compatible with the ontology
     */
    AUTORDF_ONTOLOGY_EXPORT std::shared_ptr<std::vector<Validator::Error>> validateObject(const Object& object,
                                                                                          const ValidationOption & option = ValidationOption());

private:
    std::shared_ptr<Ontology> _ontology;

    /**
     * @brief validatePropertyValue
     * @param object autordf::Object to validate
     * @param currentCLass Rdf Class to get all data property from
     * @param property
     * @param errorList list of errors
     * Fill the given error list with OWL errors found on the object Dataproperty or Annotationproperty
     */
    void validatePropertyValue(const Object& object, const std::shared_ptr<const Klass>& currentCLass,
                               const std::shared_ptr<Property>& property, std::vector<Validator::Error>* errorList);

    void validateAnnotationProperties(const Object& object, const std::shared_ptr<const Klass>& currentCLass,
                                      std::vector<Validator::Error>* errorList, const ValidationOption& option);

    /**
     * @brief validateDataProperties
     * @param object autordf::Object to validate
     * @param currentCLass Rdf Class to get all data property from
     * @param errorList list of errors
     * Fill the given error list with OWL errors found on the object Dataproperty
     */
    void validateDataProperties(const Object& object, const std::shared_ptr<const Klass>& currentCLass,
                                std::vector<Validator::Error>* errorList, const ValidationOption& option);

    /**
     * @brief validateDataKeys
     * @param object autordf::Object to validate
     * @param currentCLass Rdf Class to get all data property from
     * @param errorList list of errors
     * Fill the given error list with OWL errors found on the object KeyData
     */
    void validateDataKeys(const std::shared_ptr<const Klass>& currentClass, std::vector<Validator::Error>* errorList);

    /**
     * @brief validateDomainProperties
     * @param object autordf::Object to validate
     * @param currentCLass Rdf Class to get all data property from
     * @param errorList list of errors
     * Fill the given error list with OWL errors found on a domain of a property
     */
    void validateDomainProperties(const Object& object, const std::shared_ptr<const Klass>& currentClass,
                                      const std::shared_ptr<Property>& property, std::vector<Validator::Error> *errorList);

    /**
     * @brief validateObjectProperty
     * @param object autordf::Object to validate
     * @param currentCLass Rdf Class to get all data property from
     * @param errorList list of errors
     * Fill the given error list with OWL errors found on the object's objectProperties
     */
    void validateObjectProperties(const Object& object, const std::shared_ptr<const Klass>&,
            std::vector<Validator::Error>* errorList, const ValidationOption& option);

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
