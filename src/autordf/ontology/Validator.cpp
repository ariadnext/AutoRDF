#include <boost/date_time.hpp>
#include <functional>

#include "autordf/ontology/Validator.h"

namespace autordf {
namespace ontology {


std::string Validator::Error::fullMessage() const {
    std::string str(message);

    std::string placeholder_1("@property");
    std::string placeholder_2("@count");
    std::string placeholder_3("@val");
    std::string placeholder_5("@range");
    std::string placeholder_4("@subject");

    std::size_t foundPlaceholder_1 = str.find(placeholder_1);
    if(foundPlaceholder_1 != std::string::npos) {
        str.replace(foundPlaceholder_1, placeholder_1.length(), property.QName());
    }
    std::size_t foundPlaceholder_2 = str.find(placeholder_2);
    if(foundPlaceholder_2 != std::string::npos) {
        str.replace(foundPlaceholder_2, placeholder_2.length(), std::to_string(count));
    }
    std::size_t foundPlaceholder_3 = str.find(placeholder_3);
    if(foundPlaceholder_3 != std::string::npos) {
        str.replace(foundPlaceholder_3, placeholder_3.length(), std::to_string(val));
    }
    std::size_t foundPlaceholder_4 = str.find(placeholder_4);
    if(foundPlaceholder_4 != std::string::npos) {
        str.replace(foundPlaceholder_4, placeholder_4.length(),
                    nodeTextLocation(subject));
    }
    std::size_t foundPlaceholder_5 = str.find(placeholder_5);
    if(foundPlaceholder_5 != std::string::npos) {
        str.replace(foundPlaceholder_5, placeholder_5.length(), range);
    }

    return str;
}

std::shared_ptr<std::vector<Validator::Error>> Validator::validateModel(const Model& model) {
    std::vector<Validator::Error>  errorList;
    for ( auto uriKlass : _ontology->classUri2Ptr()) {
        std::vector<Object> objects = Object::findByType(uriKlass.first);
        for (auto const& object: objects) {
            std::shared_ptr<std::vector<Validator::Error> > objErrors = validateObject(object);
            for (auto const& objError: *objErrors) {
                errorList.push_back(objError);
            }
        }
    }

    return std::make_shared<std::vector<Validator::Error>> (errorList);
}

void Validator::validatePropertyValue(const Object& object, const std::shared_ptr<const Klass>& currentCLass,
                                      const std::shared_ptr<Property>& property, std::vector<Validator::Error>* errorList) {
    unsigned int maxCardinality = property->maxCardinality(*currentCLass);
    unsigned int minCardinality = property->minCardinality(*currentCLass);
    Uri range = property->range(currentCLass.get());
    std::vector<PropertyValue> propList = object.getPropertyValueList(property->rdfname(), false);
    Validator::Error error(object, property->rdfname());
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
                error.message = "\'@subject\' property \'@property\' value \'" + prop + "\' is not convertible as @range";
                error.range = cvt::rdfTypeEnumXMLString(rdfTypeEnum);
                errorList->push_back(error);
            }
        }
    }
}

void Validator::validateAnnotationProperties(const Object& object, const std::shared_ptr<const Klass>& currentCLass,
                                       std::vector<Validator::Error>* errorList) {

    for (auto const &annotationProperty: currentCLass->annotationProperties()) {
        validatePropertyValue(object, currentCLass, annotationProperty, errorList);
    }
}

void Validator::validateDataProperties(const Object& object, const std::shared_ptr<const Klass>& currentCLass,
                                       std::vector<Validator::Error>* errorList) {

    for (auto const &dataProperty: currentCLass->dataProperties()) {
        validatePropertyValue(object, currentCLass, dataProperty, errorList);
    }
}

void Validator::validateObjectProperties(const Object& object, const std::shared_ptr<const Klass>& currentClass,
                                         std::vector<Validator::Error>* errorList) {

    for (auto const& objectProperty: currentClass->objectProperties()) {
        unsigned int maxCardinality = objectProperty->maxCardinality(*currentClass);
        unsigned int minCardinality = objectProperty->minCardinality(*currentClass);
        autordf::Uri range = objectProperty->range(currentClass.get());
        std::vector<Object> objList = object.getObjectList(objectProperty->rdfname(), false);
        Validator::Error error(object, objectProperty->rdfname());
        error.count = objList.size();
        if(objList.size() > maxCardinality) {
            error.type = error.TOOMANYVALUES;
            error.message = "\'@subject\' property \'@property\' has @count distinct values. Maximum allowed is @val";
            error.val = maxCardinality;
            errorList->push_back(error);
        }
        if (objList.size() < minCardinality) {
            error.type = error.NOTENOUHVALUES;
            error.message = "\'@subject\' property \'@property\' has @count distinct values. Minimum allowed is @val";
            error.val = minCardinality;
            errorList->push_back(error);
        }
        for (auto const& subObj: objList) {
            if (!isObjectTypeValid(subObj, range)) {
                error.subject = subObj.iri().empty() ? object : subObj;
                std::stringstream ss;
                std::vector<PropertyValue> types = subObj.getPropertyValueList(Object::RDF_TYPE, false);
                if (types.size()) {
                    ss << "\'@subject\' property \'@property\' is of incompatible object type.";
                    ss << " RDF expected type is @range while provided type is {";
                    for ( auto it = types.begin(); it != types.end(); ++it ) {
                        ss << *it;
                        if ( it != types.end() ) {
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

std::shared_ptr<std::vector<Validator::Error>> Validator::validateObject(const Object& object) {
    std::vector<Validator::Error>  errorList;
    std::vector<Uri> types = object.getTypes(_ontology->model()->baseUri());
    std::set<Klass> allKlass;
    for (const Uri& type: types) {
        if (_ontology->containsClass(type)) {
            std::shared_ptr<const Klass> uriKlass =  _ontology->findClass(type);
            allKlass.insert(*uriKlass);
            for(auto const& ancestor: uriKlass->getAllAncestors()) {
                allKlass.insert(*ancestor);
            }
        }
    }
    for(const Klass& kl : allKlass ) {
        auto klPtr = std::make_shared<const Klass>(kl);
        validateAnnotationProperties(object, klPtr, &errorList);
        validateDataProperties(object, klPtr, &errorList);
        validateObjectProperties(object, klPtr, &errorList);

    }
    return std::make_shared<std::vector<Validator::Error>> (errorList);
}

bool Validator::isDataTypeValid(const autordf::PropertyValue& property, const autordf::cvt::RdfTypeEnum& rdfType) {
    bool valid = true;

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
                property.get < cvt::RdfTypeEnum::xsd_integer, long long int>();
                break;
            case autordf::cvt::RdfTypeEnum::xsd_nonNegativeInteger:
                property.get < cvt::RdfTypeEnum::xsd_integer, long long int>();
                break;
            case autordf::cvt::RdfTypeEnum::xsd_positiveInteger:
                property.get < cvt::RdfTypeEnum::xsd_positiveInteger, long long unsigned int>();
                break;
            case autordf::cvt::RdfTypeEnum::xsd_nonPositiveInteger:
                property.get < cvt::RdfTypeEnum::xsd_nonPositiveInteger, long long int>();
                break;
            case autordf::cvt::RdfTypeEnum::xsd_negativeInteger:
                property.get<cvt::RdfTypeEnum::xsd_negativeInteger, long long int>() ;
                break;
            case autordf::cvt::RdfTypeEnum::xsd_long:
                property.get<cvt::RdfTypeEnum::xsd_long, long long int >();
                break;
            case autordf::cvt::RdfTypeEnum::xsd_unsignedLong:
                property.get<cvt::RdfTypeEnum::xsd_unsignedLong, long long unsigned int >();
                break;
            case autordf::cvt::RdfTypeEnum::xsd_unsignedInt:
                property.get<cvt::RdfTypeEnum::xsd_unsignedInt, long unsigned int>();
                break;
            case autordf::cvt::RdfTypeEnum::xsd_int:
                property.get<cvt::RdfTypeEnum::xsd_int,long int>();
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
        valid = false;
    }

    return valid;
}

bool Validator::isObjectTypeValid(const autordf::Object& object, const autordf::Uri& type) {
    if (object.isA(type)) {
        return true;
    } else {
        std::shared_ptr<const autordf::ontology::Klass> klass =  _ontology->findClass(type);
        if (klass != nullptr) {
            for (auto predecessor: klass->getAllPredecessors()) {
                if (object.isA(predecessor->rdfname())) {
                    return true;
                }
            }
        }
    }
    return false;
}

/**
 * Compute helper string to help find location of given object
 */
std::string Validator::nodeTextLocation(const autordf::Object& obj, int maxRecurse) {
    if ( maxRecurse > 0 ) {
        if ( obj.iri().empty() ) {
            std::set<autordf::Object> sources = obj.findSources();
            for ( const autordf::Object& source : sources ) {
                std::string loc = nodeTextLocation(source, maxRecurse - 1);
                if ( !loc.empty() ) {
                    return loc + "-->Blank node";
                }
            }
            return "";
        } else {
            return obj.QName();
        }
    } else {
        return "";
    }
}

}
}