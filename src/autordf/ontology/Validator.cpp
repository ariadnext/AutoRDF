#include <boost/date_time.hpp>
#include <functional>

#include "autordf/ontology/Validator.h"

namespace autordf {
namespace ontology {
namespace validation {

/**
 * Compute helper string to get location string for a given object
 */
std::string nodeTextLocation(const autordf::Object& obj, int maxRecurse = 5) {
    if (maxRecurse <= 0) {
        return {};
    }

    if (obj.iri().empty()) {
        std::set<autordf::Object> sources = obj.findSources();
        for (const autordf::Object& source: sources) {
            std::string loc = nodeTextLocation(source, maxRecurse - 1);
            if (!loc.empty()) {
                return loc + "-->Blank node";
            }
        }
        return {};
    }
    return obj.QName();
}

std::string validation::Error::fullMessage() const {
    std::string str(message);

    std::string placeholder_1("@property");
    std::string placeholder_2("@count");
    std::string placeholder_3("@val");
    std::string placeholder_5("@range");
    std::string placeholder_4("@subject");

    std::size_t foundPlaceholder_1 = str.find(placeholder_1);
    if (foundPlaceholder_1 != std::string::npos) {
        str.replace(foundPlaceholder_1, placeholder_1.length(), property.QName());
    }
    std::size_t foundPlaceholder_2 = str.find(placeholder_2);
    if (foundPlaceholder_2 != std::string::npos) {
        str.replace(foundPlaceholder_2, placeholder_2.length(), std::to_string(count));
    }
    std::size_t foundPlaceholder_3 = str.find(placeholder_3);
    if (foundPlaceholder_3 != std::string::npos) {
        str.replace(foundPlaceholder_3, placeholder_3.length(), std::to_string(val));
    }
    std::size_t foundPlaceholder_4 = str.find(placeholder_4);
    if (foundPlaceholder_4 != std::string::npos) {
        str.replace(foundPlaceholder_4, placeholder_4.length(),
                    nodeTextLocation(subject));
    }
    std::size_t foundPlaceholder_5 = str.find(placeholder_5);
    if (foundPlaceholder_5 != std::string::npos) {
        str.replace(foundPlaceholder_5, placeholder_5.length(), range);
    }

    return str;
}

/**
 * @brief isDataTypeValid
 * @param property PropertyValue to check type from
 * @param rdfType expectd Rdf type for the property value
 * return true if the property value has the correct type, false otherwise
 */
bool isDataTypeValid(const autordf::PropertyValue& property, const autordf::cvt::RdfTypeEnum& rdfType) {
    try {
        switch (rdfType) {
            case autordf::cvt::RdfTypeEnum::xsd_boolean:
                property.get<cvt::RdfTypeEnum::xsd_boolean, bool>();
                break;
            case autordf::cvt::RdfTypeEnum::xsd_decimal:
                property.get<cvt::RdfTypeEnum::xsd_decimal, double>();
                break;
            case autordf::cvt::RdfTypeEnum::xsd_float:
                property.get<cvt::RdfTypeEnum::xsd_float, float>();
                break;
            case autordf::cvt::RdfTypeEnum::xsd_double:
                property.get<autordf::cvt::RdfTypeEnum::xsd_double, double>();
                break;
            case autordf::cvt::RdfTypeEnum::xsd_integer:
                property.get<cvt::RdfTypeEnum::xsd_integer, long long int>();
                break;
            case autordf::cvt::RdfTypeEnum::xsd_nonNegativeInteger:
                property.get<cvt::RdfTypeEnum::xsd_integer, long long int>();
                break;
            case autordf::cvt::RdfTypeEnum::xsd_positiveInteger:
                property.get<cvt::RdfTypeEnum::xsd_positiveInteger, long long unsigned int>();
                break;
            case autordf::cvt::RdfTypeEnum::xsd_nonPositiveInteger:
                property.get<cvt::RdfTypeEnum::xsd_nonPositiveInteger, long long int>();
                break;
            case autordf::cvt::RdfTypeEnum::xsd_negativeInteger:
                property.get<cvt::RdfTypeEnum::xsd_negativeInteger, long long int>();
                break;
            case autordf::cvt::RdfTypeEnum::xsd_long:
                property.get<cvt::RdfTypeEnum::xsd_long, long long int>();
                break;
            case autordf::cvt::RdfTypeEnum::xsd_unsignedLong:
                property.get<cvt::RdfTypeEnum::xsd_unsignedLong, long long unsigned int>();
                break;
            case autordf::cvt::RdfTypeEnum::xsd_unsignedInt:
                property.get<cvt::RdfTypeEnum::xsd_unsignedInt, long unsigned int>();
                break;
            case autordf::cvt::RdfTypeEnum::xsd_int:
                property.get<cvt::RdfTypeEnum::xsd_int, long int>();
                break;
            case autordf::cvt::RdfTypeEnum::xsd_short:
                property.get<cvt::RdfTypeEnum::xsd_short, short>();
                break;
            case autordf::cvt::RdfTypeEnum::xsd_unsignedShort:
                property.get<cvt::RdfTypeEnum::xsd_unsignedShort, unsigned short>();
                break;
            case autordf::cvt::RdfTypeEnum::xsd_dateTime:
                property.get<cvt::RdfTypeEnum::xsd_dateTime, boost::posix_time::ptime>();
                break;
            case autordf::cvt::RdfTypeEnum::xsd_dateTimeStamp:
                property.get<cvt::RdfTypeEnum::xsd_dateTime, boost::posix_time::ptime>();
                break;
            case autordf::cvt::RdfTypeEnum::xsd_string:
                property.get<cvt::RdfTypeEnum::xsd_string, std::string>();
                break;
            case autordf::cvt::RdfTypeEnum::xsd_unsignedByte:
                property.get<cvt::RdfTypeEnum::xsd_unsignedByte, unsigned char>();
                break;
            case autordf::cvt::RdfTypeEnum::xsd_byte:
                property.get<cvt::RdfTypeEnum::xsd_byte, char>();
                break;
            case autordf::cvt::RdfTypeEnum::rdf_langString:
                property.get<cvt::RdfTypeEnum::rdf_langString, autordf::I18String>();
                break;
        }
    } catch (const DataConvertionFailure&) {
        return false;
    }
    return true;
}

/**
 * @brief validatePropertyValue
 * @param object autordf::Object to validate
 * @param currentCLass Rdf Class to get all data property from
 * @param property
 * @param errorList list of errors
 * Fill the given error list with OWL errors found on the object Dataproperty or Annotationproperty
 */
void validatePropertyValue(const Object& object, const Klass& currentCLass,
                           const Property& property, std::vector<validation::Error> *errorList) {
    unsigned int maxCardinality = property.maxCardinality(currentCLass);
    unsigned int minCardinality = property.minCardinality(currentCLass);
    Uri range = property.range(&currentCLass);
    std::vector<PropertyValue> propList = object.getPropertyValueList(property.rdfname(), false);
    validation::Error error(object, property.rdfname());
    error.count = propList.size();
    if (propList.size() > maxCardinality) {
        error.type = error.TOOMANYVALUES;
        error.message = "\'@subject\' property \'@property\' has @count distinct values. Maximum allowed is @val";
        error.val = maxCardinality;
        errorList->push_back(error);
    }
    if (propList.size() < minCardinality) {
        error.type = error.NOTENOUHVALUES;
        error.message = "\'@subject\' property \'@property\' has @count distinct values. Minimum allowed is @val";
        error.val = minCardinality;
        errorList->push_back(error);
    }

    auto rdfType = cvt::rdfMapType.find(range);
    if (rdfType != autordf::cvt::rdfMapType.end()) {
        autordf::cvt::RdfTypeEnum rdfTypeEnum = rdfType->second;
        for (auto const& prop: propList) {
            if (!isDataTypeValid(prop, rdfTypeEnum)) {
                error.type = error.INVALIDDATATYPE;
                error.message =
                        "\'@subject\' property \'@property\' value \'" + prop + "\' is not convertible as @range";
                error.range = cvt::rdfTypeEnumXMLString(rdfTypeEnum);
                errorList->push_back(error);
            }
        }
    }
}

/**
 * @brief validateDomainProperties
 * @param object autordf::Object to validate
 * @param currentCLass Rdf Class to get all data property from
 * @param errorList list of errors
 * Fill the given error list with OWL errors found on a domain of a property
 */
void validateDomainProperties(const Object& object, const Klass& currentClass,
                              const Property& property, std::vector<validation::Error> *errorList) {
    validation::Error error(object, property.rdfname());
    // Verify that the class is in the property domain list
    auto domains = property.domains();
    if (std::find(domains.begin(), domains.end(), currentClass.rdfname()) == domains.end()) {
        error.type = Error::INVALIDDOMAIN;
        error.message = currentClass.rdfname().prettyName()
                        + " class should be in the \'@property\' domain";
        errorList->push_back(error);
    }
}

void
validateAnnotationProperties(const Object& object, const Klass& currentCLass, const validation::ValidationOption& option,
                             std::vector<validation::Error> *errorList) {

    for (auto const& annotationProperty: currentCLass.annotationProperties()) {
        validatePropertyValue(object, currentCLass, *annotationProperty, errorList);
        if (option.enforceExplicitDomains) {
            validateDomainProperties(object, currentCLass, *annotationProperty, errorList);
        }
    }
}

/**
 * @brief validateDataProperties
 * @param object autordf::Object to validate
 * @param currentCLass Rdf Class to get all data property from
 * @param errorList list of errors
 * Fill the given error list with OWL errors found on the object Dataproperty
 */
void validateDataProperties(const Object& object, const Klass& currentCLass, const validation::ValidationOption& option,
                            std::vector<validation::Error> *errorList) {
    for (auto const& dataProperty: currentCLass.dataProperties()) {
        validatePropertyValue(object, currentCLass, *dataProperty, errorList);
        if (option.enforceExplicitDomains) {
            validateDomainProperties(object, currentCLass, *dataProperty, errorList);
        }
    }
}

/**
 * @brief validateDataKeys
 * @param object autordf::Object to validate
 * @param currentCLass Rdf Class to get all data property from
 * @param errorList list of errors
 * Fill the given error list with OWL errors found on the object KeyData
 */
void validateDataKeys(const Klass& currentClass,
                      std::vector<validation::Error> *errorList) {
    auto dataKeys = currentClass.dataKeys();
    if (dataKeys.empty()) {
        return;
    }
    std::vector<Object> objList = Object::findByType(currentClass.rdfname());
    std::map<Uri, std::list<PropertyValue>> propListAll;

    for (auto const& obj: objList) {
        for (auto const& dataKey: dataKeys) {
            std::vector<PropertyValue> propListCurrent = obj.getPropertyValueList(dataKey->rdfname(), false);
            auto& propList = propListAll[dataKey->rdfname()];
            propList.insert(propList.end(), propListCurrent.begin(), propListCurrent.end());

        }
    }

    for (auto& prop: propListAll) {
        prop.second.sort();
        auto duplicated = std::adjacent_find(prop.second.begin(), prop.second.end());
        if (duplicated != prop.second.end()) {
            validation::Error error(currentClass.rdfname(), prop.first);
            error.type = validation::Error::DUPLICATEDVALUESKEY;
            error.message = "\'@subject\' class key \'@property\' has the duplicated value \'" + *duplicated + '\'';
            errorList->push_back(error);
        }
    }
}

/**
 * @brief isObjectTypeValid
 * @param object the object to check type
 * @param type expected type of the object
 * return true if the given object is of the given type or type's subclass, false otherwise
 */
bool isObjectTypeValid(const Ontology& ontology,
                       const autordf::Object& object, const autordf::Uri& type) {
    if (object.isA(type)) {
        return true;
    } else {
        std::shared_ptr<const autordf::ontology::Klass> klass = ontology.findClass(type);
        if (klass) {
            for (const auto& predecessor: klass->getAllPredecessors()) {
                if (object.isA(predecessor->rdfname())) {
                    return true;
                }
            }
        }
    }
    return false;
}

/**
 * @brief validateObjectProperty
 * @param object autordf::Object to validate
 * @param currentCLass Rdf Class to get all data property from
 * @param errorList list of errors
 * Fill the given error list with OWL errors found on the object's objectProperties
 */
void validateObjectProperties(const Ontology& ontology, const Object& object, const Klass& currentClass,
                              const validation::ValidationOption& option, std::vector<validation::Error> *errorList) {

    for (auto const& objectProperty: currentClass.objectProperties()) {
        if (option.enforceExplicitDomains) {
            validateDomainProperties(object, currentClass, *objectProperty, errorList);
        }
        unsigned int maxCardinality = objectProperty->maxCardinality(currentClass);
        unsigned int minCardinality = objectProperty->minCardinality(currentClass);
        autordf::Uri range = objectProperty->range(&currentClass);
        std::vector<Object> objList = object.getObjectList(objectProperty->rdfname(), false);
        validation::Error error(object, objectProperty->rdfname());
        error.count = (int) objList.size();
        if (objList.size() > maxCardinality) {
            error.type = error.TOOMANYVALUES;
            error.message = "\'@subject\' property \'@property\' has @count distinct values. Maximum allowed is @val";
            error.val = (int) maxCardinality;
            errorList->push_back(error);
        }
        if (objList.size() < minCardinality) {
            error.type = error.NOTENOUHVALUES;
            error.message = "\'@subject\' property \'@property\' has @count distinct values. Minimum allowed is @val";
            error.val = (int) minCardinality;
            errorList->push_back(error);
        }
        for (auto const& subObj: objList) {
            if (!isObjectTypeValid(ontology, subObj, range)) {
                error.subject = subObj.iri().empty() ? object : subObj;
                std::stringstream ss;
                std::vector<PropertyValue> types = subObj.getPropertyValueList(Object::RDF_TYPE, false);
                if (!types.empty()) {
                    ss << "\'@subject\' property \'@property\' is of incompatible object type.";
                    ss << " RDF expected type is @range while provided type is {";
                    for (auto it = types.begin(); it != types.end(); ++it) {
                        ss << *it;
                        if (it != types.end()) {
                            ss << ", ";
                        } else {
                            ss << "}";
                        }
                    }
                } else {
                    ss << "\'@subject\' property \'@property\' has no type, while RDF expected type is @range";
                }
                error.message = ss.str();
                error.range = range;
                error.type = error.INVALIDTYPE;
                errorList->push_back(error);
            }
        }
    }
}

std::vector<validation::Error> validateObject(const Ontology& ontology,
                                              const Object& object,
                                              const validation::ValidationOption& option) {
    Error::Vec errorList;
    std::vector<Uri> types = object.getTypes(ontology.model()->baseUri());
    std::set<Klass> allKlass;
    for (const Uri& type: types) {
        if (ontology.containsClass(type)) {
            std::shared_ptr<const Klass> uriKlass = ontology.findClass(type);
            allKlass.insert(*uriKlass);
            for (auto const& ancestor: uriKlass->getAllAncestors()) {
                allKlass.insert(*ancestor);
            }
        }
    }
    for (const Klass& kl: allKlass) {
        validateAnnotationProperties(object, kl, option, &errorList);
        validateDataProperties(object, kl, option, &errorList);
        validateObjectProperties(ontology, object, kl, option, &errorList);
        if (option.enforceObjectKeyUniqueness) {
            validateDataKeys(kl, &errorList);
        }
    }
    return errorList;
}

std::vector<validation::Error> validateModel(const Ontology& ontology,
                                             const validation::ValidationOption& option) {
    Error::Vec errorList;
    for (const auto& uriKlass: ontology.classUri2Ptr()) {
        std::vector<Object> objects = Object::findByType(uriKlass.first);
        for (auto const& object: objects) {
            Error::Vec objErrors = validateObject(ontology, object, option);
            errorList.insert(errorList.end(), objErrors.begin(), objErrors.end());
        }
    }

    return errorList;
}

}
}
}
