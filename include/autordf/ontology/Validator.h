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
 * Provides methods to check if a model or an object is compatible with a given ontology
 */
namespace validation {
/**
 * Each error in given dataset is reported in an Error structure
 */
class Error {
public:
    using Vec = std::vector<Error>;
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
    unsigned int count;

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
AUTORDF_ONTOLOGY_EXPORT Error::Vec validateModel(const Ontology& ontology,
                                                 const ValidationOption& option = ValidationOption());

/**
 * Checks this object is compatible with ontology,
 * that means all its properties are compatible with the ontology
 */
AUTORDF_ONTOLOGY_EXPORT Error::Vec validateObject(const Ontology& ontology,
                                                  const Object& object,
                                                  const ValidationOption& option = ValidationOption());
}
}
}

#endif //AUTORDF_ONTOLOGY_VALIDATOR_H
