#include <boost/date_time.hpp>
#include <functional>

#include "autordf/ontology/Validator.h"

namespace autordf {
namespace ontology {


std::string Validator::Error::fullMessage() {
    std::string str(message);

    std::string placeholder_1("@property");
    std::string placeholder_2("@cardinality");
    std::string placeholder_3("@count");
    std::string placeholder_4("@subject");

    std::size_t foundPlaceholder_1 = str.find(placeholder_1);
    if(foundPlaceholder_1 != std::string::npos) {
        str.replace(foundPlaceholder_1, placeholder_1.length(), property);
    }
    std::size_t foundPlaceholder_2 = str.find(placeholder_2);
    if(foundPlaceholder_2 != std::string::npos) {
        str.replace(foundPlaceholder_2, placeholder_2.length(), std::to_string(cardinality));
    }
    std::size_t foundPlaceholder_3 = str.find(placeholder_3);
    if(foundPlaceholder_3 != std::string::npos) {
        str.replace(foundPlaceholder_3, placeholder_3.length(), std::to_string(count));
    }
    std::size_t foundPlaceholder_4 = str.find(placeholder_4);
    if(foundPlaceholder_4 != std::string::npos) {
        str.replace(foundPlaceholder_4, placeholder_4.length(), subject.iri());
    }

    return str;
}

std::shared_ptr<std::vector<Validator::Error>> Validator::validateModel(const Model& model) {

    std::shared_ptr<std::vector<Validator::Error>>  errorList;
    return errorList;
}

std::vector<Validator::Error> Validator::validateObject(const Object& object) {
    std::vector<Validator::Error>  errorList;
    std::vector<Uri> types = object.getTypes();

    for (auto const& type: types) {
        for ( auto uriKlass : _ontology.classUri2Ptr()) {
            if (type == uriKlass.first) {
                Validator::Error error;
                error.subject = object;
                for (auto const &dataProperty: uriKlass.second->dataProperties()) {
                    unsigned int maxCardinality = dataProperty->maxCardinality(*uriKlass.second);
                    unsigned int minCardinality = dataProperty->minCardinality(*uriKlass.second);
                    Uri range = dataProperty->range(uriKlass.second.get());
                    std::vector<PropertyValue> propList = object.getPropertyValueList(dataProperty->rdfname());
                    error.property = dataProperty->rdfname();
                    error.count = propList.size();
                    if (propList.size() > maxCardinality) {
                        error.type = error.TOOMANYVALUES;
                        error.message = "Property @property has too many values: @count. Maximum allowed is @cardinality";
                        error.cardinality = maxCardinality;
                        errorList.push_back(error);
                    }
                    if (propList.size() < minCardinality) {
                        error.type = error.NOTENOUHVALUES;
                        error.message = "Property @property doesn't have enough values: @count. Minimum allowed is @cardinality" ;
                        error.cardinality = minCardinality;
                        errorList.push_back(error);
                    }

                    autordf::cvt::RdfTypeEnum rdfTypeEnum;
                    for (auto const& prop: propList) {
                        auto rdfType = cvt::rdfMapType.find(range);
                        if (rdfType != autordf::cvt::rdfMapType.end()) {
                            rdfTypeEnum = rdfType->second;
                        }
                        if (!isDatatypeValid(prop, rdfTypeEnum)) {
                            error.type = error.INVALIDDATATYPE;
                            error.message = "Rdf type for this property @property not allowed. Rdf type required: " + cvt::rdfTypeEnumXMLString(rdfTypeEnum);
                            errorList.push_back(error);
                        }
                    }
                }

                for (auto const& objectProperty: uriKlass.second->objectProperties()) {
                    unsigned int maxCardinality = objectProperty->maxCardinality(*uriKlass.second);
                    unsigned int minCardinality = objectProperty->minCardinality(*uriKlass.second);
                    autordf::Uri range = objectProperty->range(uriKlass.second.get());
                    std::vector<Object> objList = object.getObjectList(objectProperty->rdfname());
                    error.property = objectProperty->rdfname();
                    error.count = objList.size();
                    if(objList.size() > maxCardinality) {
                        error.type = error.TOOMANYVALUES;
                        error.cardinality = maxCardinality;
                        error.message =  "Property @property has too many values: @count. Maximum allowed is @cardinality";
                        errorList.push_back(error);
                    }
                    if (objList.size() < minCardinality) {
                        error.type = error.NOTENOUHVALUES;
                        error.cardinality = minCardinality;
                        error.message = "Property @property doesn't have enough values: @count. Minimum allowed is @cardinality" ;
                        errorList.push_back(error);
                    }
                    for (auto const& object: objList) {
                        if (!object.isA(range)) {
                            error.message = "Rdf type for this property @property not allowed. Rdf type required: " + range;
                            error.type = error.INVALIDTYPE;
                            errorList.push_back(error);
                        }
                    }
                }
                break;
            }
        }
    }
    return errorList;
}

bool Validator::isDatatypeValid(const autordf::PropertyValue& property, const autordf::cvt::RdfTypeEnum& rdfType) {
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
                    property.get < cvt::RdfTypeEnum::xsd_integer, long
                    long
                    int > ();
                break;
            case autordf::cvt::RdfTypeEnum::xsd_nonNegativeInteger:
                    property.get < cvt::RdfTypeEnum::xsd_integer, long
                    long
                    int > ();
                break;
            case autordf::cvt::RdfTypeEnum::xsd_positiveInteger:
                    property.get < cvt::RdfTypeEnum::xsd_positiveInteger, long
                    long
                    unsigned
                    int > ();
                break;
            case autordf::cvt::RdfTypeEnum::xsd_nonPositiveInteger:
                    property.get < cvt::RdfTypeEnum::xsd_nonPositiveInteger, long
                    long
                    int > ();
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

        }
    } catch (const DataConvertionFailure& e) {
        valid = false;
    }

    return valid;
}


}
}