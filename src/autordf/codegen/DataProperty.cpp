#include "DataProperty.h"

#include <autordf/cvt/Cvt.h>

#include "Klass.h"
#include "Utils.h"

namespace autordf {
namespace codegen {

struct CppType {
public:
    CppType(const std::string& oneCppType, const std::string& manyCppType) {
        undefined = oneCppType.empty();
        oneType = oneCppType.empty() ? "autordf::PropertyValue" : oneCppType;
        manyType = manyCppType.empty() ? "std::vector<" + oneType + ">" : manyCppType;
    }
    bool undefined = true;
    std::string oneType;
    std::string manyType;
};

std::map<cvt::RdfTypeEnum, CppType> rdf2CppTypeMapping(
        {
//FIXME: handle PlainLiteral, XMLLiteral, Literal, real, rational
        std::pair<cvt::RdfTypeEnum, CppType>(cvt::RdfTypeEnum::xsd_string,             {"std::string",               ""}),
        std::pair<cvt::RdfTypeEnum, CppType>(cvt::RdfTypeEnum::xsd_boolean,            {"bool",                      ""}),
        std::pair<cvt::RdfTypeEnum, CppType>(cvt::RdfTypeEnum::xsd_decimal,            {"double",                    ""}),
        std::pair<cvt::RdfTypeEnum, CppType>(cvt::RdfTypeEnum::xsd_float,              {"float",                     ""}),
        std::pair<cvt::RdfTypeEnum, CppType>(cvt::RdfTypeEnum::xsd_double,             {"double",                    ""}),
        std::pair<cvt::RdfTypeEnum, CppType>(cvt::RdfTypeEnum::xsd_dateTime,           {"boost::posix_time::ptime",  ""}),
        std::pair<cvt::RdfTypeEnum, CppType>(cvt::RdfTypeEnum::xsd_integer,            {"long long int",             ""}),
        std::pair<cvt::RdfTypeEnum, CppType>(cvt::RdfTypeEnum::xsd_dateTimeStamp,      {"boost::posix_time::ptime",  ""}),
        std::pair<cvt::RdfTypeEnum, CppType>(cvt::RdfTypeEnum::xsd_nonNegativeInteger, {"long long unsigned int",    ""}),
        std::pair<cvt::RdfTypeEnum, CppType>(cvt::RdfTypeEnum::xsd_positiveInteger,    {"long long unsigned int",    ""}),
        std::pair<cvt::RdfTypeEnum, CppType>(cvt::RdfTypeEnum::xsd_nonPositiveInteger, {"long long int",             ""}),
        std::pair<cvt::RdfTypeEnum, CppType>(cvt::RdfTypeEnum::xsd_negativeInteger,    {"long long int",             ""}),
        std::pair<cvt::RdfTypeEnum, CppType>(cvt::RdfTypeEnum::xsd_long,               {"long long int",             ""}),
        std::pair<cvt::RdfTypeEnum, CppType>(cvt::RdfTypeEnum::xsd_unsignedLong,       {"unsigned long long int",    ""}),
        std::pair<cvt::RdfTypeEnum, CppType>(cvt::RdfTypeEnum::xsd_int,                {"long int",                  ""}),
        std::pair<cvt::RdfTypeEnum, CppType>(cvt::RdfTypeEnum::xsd_unsignedInt,        {"long unsigned int",         ""}),
        std::pair<cvt::RdfTypeEnum, CppType>(cvt::RdfTypeEnum::xsd_short,              {"short",                     ""}),
        std::pair<cvt::RdfTypeEnum, CppType>(cvt::RdfTypeEnum::xsd_unsignedShort,      {"unsigned short",            ""}),
        std::pair<cvt::RdfTypeEnum, CppType>(cvt::RdfTypeEnum::xsd_byte,               {"char",                      ""}),
        std::pair<cvt::RdfTypeEnum, CppType>(cvt::RdfTypeEnum::xsd_unsignedByte,       {"unsigned char",             ""}),
        std::pair<cvt::RdfTypeEnum, CppType>(cvt::RdfTypeEnum::rdf_langString,         {"autordf::I18String",        "autordf::I18StringVector"})
    }
);

void DataProperty::generateDeclaration(std::ostream& ofs, const Klass& onClass) const {
    indent(ofs, 1) << "/**" << std::endl;
    indent(ofs, 1) << " * Full iri for " <<  _decorated.rdfname().prettyName() << " data property." << std::endl;
    indent(ofs, 1) << " */" << std::endl;
    indent(ofs, 1) << "static const char* " << _decorated.prettyIRIName() << "DataPropertyIri;" << std::endl;
    ofs << std::endl;

    if ( _decorated.maxCardinality(onClass.decorated()) <= 1 ) {
        if ( _decorated.minCardinality(onClass.decorated()) > 0 ) {
            generateGetterForOneMandatory(ofs, onClass);
        } else {
            generateGetterForOneOptional(ofs, onClass);
        }
        ofs << std::endl;
        generateSetterForOne(ofs, onClass);
    }
    if ( _decorated.maxCardinality(onClass.decorated()) > 1 ) {
        generateGetterForMany(ofs, onClass);
        ofs << std::endl;
        generateSetterForOne(ofs, onClass);
        ofs << std::endl;
        generateSetterForMany(ofs, onClass);
    }
    if ( _decorated.minCardinality(onClass.decorated()) != _decorated.maxCardinality(onClass.decorated()) ) {
        ofs << std::endl;
        generateRemover(ofs, onClass);
    }
}

void DataProperty::generateKeyDeclaration(std::ostream& ofs, const Klass& onClass) const {
    std::pair<cvt::RdfTypeEnum, CppType> rdfCppType = getRdfCppTypes(onClass);
    std::string currentClassName = onClass.decorated().prettyIRIName();

    indent(ofs, 1) << "/**" << std::endl;
    indent(ofs, 1) << " * @brief This method " << genCppNameSpaceFullyQualified() << "::" << currentClassName << "::findBy" << _decorated.prettyIRIName(true) << " returns the only instance of " + onClass.decorated().rdfname() + " with property " + _decorated.rdfname() +  " set to given value." << std::endl;
    indent(ofs, 1) << " * " << std::endl;
    indent(ofs, 1) << " * @param key value that uniquely identifies the expected object" << std::endl;
    indent(ofs, 1) << " * " << std::endl;
    indent(ofs, 1) << " * @throw DuplicateObject if more than one object have the same property value" << std::endl;
    indent(ofs, 1) << " * @throw ObjectNotFound if no object has given property with value" << std::endl;
    indent(ofs, 1) << " */" << std::endl;

    indent(ofs, 1) << "static " << currentClassName << " findBy" << _decorated.prettyIRIName(true) << "( const autordf::PropertyValue& key ) {" << std::endl;
    indent(ofs, 2) <<     "return Object::findByKey(\"" << _decorated.rdfname() << "\", key).as<" << currentClassName << ">();" << std::endl;
    indent(ofs, 1) << "}" << std::endl;
    indent(ofs, 1) << std::endl;

    if (!rdfCppType.second.undefined) {
        indent(ofs, 1) << "/**" << std::endl;
        indent(ofs, 1) << " * @brief This method " << genCppNameSpaceFullyQualified() << "::" << currentClassName << "::findBy" << _decorated.prettyIRIName(true) << " returns the only instance of " + onClass.decorated().rdfname() + " with property " + _decorated.rdfname() +  " set to given value." << std::endl;
        indent(ofs, 1) << " * " << std::endl;
        indent(ofs, 1) << " * @param key value that uniquely identifies the expected object" << std::endl;
        indent(ofs, 1) << " * " << std::endl;
        indent(ofs, 1) << " * @throw DuplicateObject if more than one object have the same property value" << std::endl;
        indent(ofs, 1) << " * @throw ObjectNotFound if no object has given property with value" << std::endl;
        indent(ofs, 1) << " */" << std::endl;

        std::string cppType = rdfCppType.second.oneType;
        indent(ofs, 1) << "static " << currentClassName << " findBy" << _decorated.prettyIRIName(true) << "( const " << cppType << "& key ) {" << std::endl;
        indent(ofs, 2) <<     "return findBy" << _decorated.prettyIRIName(true) << "(autordf::PropertyValue().set<autordf::cvt::RdfTypeEnum::" << cvt::rdfTypeEnumString(rdfCppType.first) << ">(key));" << std::endl;
        indent(ofs, 1) << "}" << std::endl;
        indent(ofs, 1) << std::endl;
    }
}

void DataProperty::generateDefinition(std::ostream& ofs, const Klass& onClass) const {
    std::string currentClassName = "I" + onClass.decorated().prettyIRIName();
    std::pair<cvt::RdfTypeEnum, CppType> rdfCppType = getRdfCppTypes(onClass);

    ofs << "const char* " << currentClassName << "::" << _decorated.prettyIRIName() << "DataPropertyIri = \"" << _decorated.rdfname() << "\";" << std::endl;
    ofs << std::endl;

    if (!rdfCppType.second.undefined) {
        cvt::RdfTypeEnum rdfType = rdfCppType.first;
        std::string oneCppType = rdfCppType.second.oneType;
        std::string manyCppType = rdfCppType.second.manyType;

        if (_decorated.maxCardinality(onClass.decorated()) == 1 &&
            _decorated.minCardinality(onClass.decorated()) == 0) {
            ofs << "std::shared_ptr<" << oneCppType << "> " << currentClassName << "::" << _decorated.prettyIRIName() << "Optional() const {" << std::endl;
            indent(ofs, 1) << "auto ptrval = object().getOptionalPropertyValue(\"" << _decorated.rdfname() << "\");" << std::endl;
            indent(ofs, 1) << "return (ptrval ? std::shared_ptr<" << oneCppType << ">(new " << oneCppType <<
                           "(ptrval->get<autordf::cvt::RdfTypeEnum::" << cvt::rdfTypeEnumString(rdfType) << ", " << oneCppType << ">())) : nullptr);" << std::endl;
            ofs << "}" << std::endl;
            ofs << std::endl;
            ofs << oneCppType << " " << currentClassName << "::" << _decorated.prettyIRIName() << "(const " << oneCppType << "& defaultval) const {" << std::endl;
            indent(ofs, 1) << "auto ptrval = object().getOptionalPropertyValue(\"" << _decorated.rdfname() << "\");" << std::endl;
            indent(ofs, 1) << "return (ptrval ? ptrval->get<autordf::cvt::RdfTypeEnum::" << cvt::rdfTypeEnumString(rdfType) << ", " << oneCppType << ">() : defaultval);" << std::endl;
            ofs << "}" << std::endl;
        }
        if (_decorated.maxCardinality(onClass.decorated()) > 1) {
            ofs << manyCppType << " " << currentClassName << "::" << _decorated.prettyIRIName() << "List() const {" << std::endl;
            indent(ofs, 1) << "return object().getValueListImpl<autordf::cvt::RdfTypeEnum::" <<
                           cvt::rdfTypeEnumString(rdfType) << ", " << oneCppType << ">(\"" << _decorated.rdfname() << "\", " << orderedBoolValue() << ");" << std::endl;
            ofs << "}" << std::endl;
            ofs << std::endl;
            ofs << currentClassName << "& " << currentClassName << "::set" << _decorated.prettyIRIName(true) << "(const " << manyCppType << "& values) {" << std::endl;
            indent(ofs, 1) <<     "object().setValueListImpl<autordf::cvt::RdfTypeEnum::" <<
            cvt::rdfTypeEnumString(rdfType) << ">(\"" << _decorated.rdfname() << "\", values, " << orderedBoolValue() << ");" << std::endl;
            indent(ofs, 1) << "return *this;" << std::endl;
            ofs << "}" << std::endl;
        }
    }
}

std::pair<cvt::RdfTypeEnum, CppType> DataProperty::getRdfCppTypes(const Klass& onClass) const {
    auto rdfTypeEntry = cvt::rdfMapType.find(_decorated.range(&onClass.decorated()));

    if (rdfTypeEntry != cvt::rdfMapType.end()) {
        auto cppTypeEntry = rdf2CppTypeMapping.find(rdfTypeEntry->second);
        if (cppTypeEntry != rdf2CppTypeMapping.end()) {
            return std::make_pair(rdfTypeEntry->second, cppTypeEntry->second);
        }
    }
    return std::make_pair(cvt::RdfTypeEnum::xsd_string , CppType("", ""));
}

void DataProperty::generateGetterForOneMandatory(std::ostream& ofs, const Klass& onClass) const {
    std::string methodName = _decorated.prettyIRIName();
    generatePropertyComment(ofs, onClass, methodName, 1,
                    "@return the mandatory value for this property.\n"
                            "@throw PropertyNotFound if value is not set in database");
    std::pair<cvt::RdfTypeEnum, CppType> rdfCppType = getRdfCppTypes(onClass);

    cvt::RdfTypeEnum rdfType = rdfCppType.first;
    std::string cppType = rdfCppType.second.oneType;
    indent(ofs, 1) << cppType << " " << methodName << "() const {" << std::endl;
    indent(ofs, 2) << "return object().getPropertyValue(\"" << _decorated.rdfname() << "\")" <<
        (rdfCppType.second.undefined ?
            ";" :
            ".get<autordf::cvt::RdfTypeEnum::" + cvt::rdfTypeEnumString(rdfType) + ", " + cppType + ">();") << std::endl;
    indent(ofs, 1) << "}" << std::endl;
}

void DataProperty::generateSetterForOne(std::ostream& ofs, const Klass& onClass) const {
    std::string methodName = "set" + _decorated.prettyIRIName(true);
    generatePropertyComment(ofs, onClass, methodName, 1,
                    "Sets the mandatory value for this property.\n"
                            "@param value value to set for this property, removing all other values");
    std::pair<cvt::RdfTypeEnum, CppType> rdfCppType = getRdfCppTypes(onClass);

    std::string currentClassName = "I" + onClass.decorated().prettyIRIName();

    std::string cppType = rdfCppType.second.oneType;
    cvt::RdfTypeEnum rdfType = rdfCppType.first;
    indent(ofs, 1) << currentClassName << "& " << methodName << "(const " << cppType << "& value) {" << std::endl;
    indent(ofs, 2) << "object().setPropertyValue(\"" << _decorated.rdfname() << "\", " <<
        (!rdfCppType.second.undefined ?
         "autordf::PropertyValue().set<autordf::cvt::RdfTypeEnum::" + cvt::rdfTypeEnumString(rdfType) + ">(value));" :
         "value);") << std::endl;
    indent(ofs, 2) << "return *this;" << std::endl;
    indent(ofs, 1) << "}" << std::endl;
}

void DataProperty::generateGetterForOneOptional(std::ostream& ofs, const Klass& onClass) const {
    std::pair<cvt::RdfTypeEnum, CppType> rdfCppType = getRdfCppTypes(onClass);

    std::string methodName = _decorated.prettyIRIName() + "Optional";
    generatePropertyComment(ofs, onClass, methodName, 1,
                    "@return the valueif it is set, or nullptr if it is not set.");

    std::string cppType = rdfCppType.second.oneType;
    indent(ofs, 1) << "std::shared_ptr<" << cppType << "> " << methodName << "() const" << (!rdfCppType.second.undefined ? ";" : " {") << std::endl;
    if (rdfCppType.second.undefined) {
        indent(ofs, 2) << "return object().getOptionalPropertyValue(\"" << _decorated.rdfname() << "\");" << std::endl;
        indent(ofs, 1) << "}" << std::endl;
    }

    ofs << std::endl;
    methodName = _decorated.prettyIRIName();
    generatePropertyComment(ofs, onClass, methodName, 1,
                    "@return the valueif it is set, or defaultval if it is not set.");

    indent(ofs, 1) << cppType << " " << _decorated.prettyIRIName() << "(const " << cppType << "& defaultval) const" << (!rdfCppType.second.undefined ? ";" : " {") << std::endl;
    if (rdfCppType.second.undefined) {
        indent(ofs, 2) << "auto ptr = object().getOptionalPropertyValue(\"" << _decorated.rdfname() << "\");" << std::endl;
        indent(ofs, 2) << "return (ptr ? *ptr : defaultval);" << std::endl;
        indent(ofs, 1) << "}" << std::endl;
    }
}

void DataProperty::generateGetterForMany(std::ostream& ofs, const Klass& onClass) const {
    std::string methodName = _decorated.prettyIRIName() + "List";
    generatePropertyComment(ofs, onClass, methodName, 1,
                    "@return the list of values.  List can be empty if not values are set in database");
    std::pair<cvt::RdfTypeEnum, CppType> rdfCppType = getRdfCppTypes(onClass);
    std::string cppType = rdfCppType.second.manyType;
    indent(ofs, 1) << cppType << " " << methodName << "() const" << (!rdfCppType.second.undefined ? ";" : " {") << std::endl;
    if(rdfCppType.second.undefined) {
        indent(ofs, 2) <<     "return object().getPropertyValueList(\"" << _decorated.rdfname() << "\", " << orderedBoolValue() << ");" << std::endl;
        indent(ofs, 1) << "}" << std::endl;
    }
}

void DataProperty::generateSetterForMany(std::ostream& ofs, const Klass& onClass) const {
    std::string methodName = "set" + _decorated.prettyIRIName(true);
    generatePropertyComment(ofs, onClass, methodName, 1,
                    "Sets property to list of values \n@param values the list of values");
    std::pair<cvt::RdfTypeEnum, CppType> rdfCppType = getRdfCppTypes(onClass);

    std::string currentClassName = "I" + onClass.decorated().prettyIRIName();

    std::string cppManyType = rdfCppType.second.manyType;
    indent(ofs, 1) << currentClassName << "& " << methodName << "(const " << cppManyType << " " << "& values)" << (!rdfCppType.second.undefined ? ";" : " {") << std::endl;
    if (rdfCppType.second.undefined) {
        indent(ofs, 2) <<     "object().setPropertyValueList(\"" << _decorated.rdfname() << "\", values, " << orderedBoolValue() << ");" << std::endl;
        indent(ofs, 2) <<     "return *this;" << std::endl;
        indent(ofs, 1) << "}" << std::endl;
    }
    ofs << std::endl;

    methodName = "add" + _decorated.prettyIRIName(true);
    generatePropertyComment(ofs, onClass, methodName, 1,
                    "Adds a value to a property \n@param value the value to add");

    std::string cppOneType = rdfCppType.second.oneType;
    cvt::RdfTypeEnum rdfType = rdfCppType.first;
    indent(ofs, 1) << currentClassName << "& " << methodName << "(const " << cppOneType << "& value) {" << std::endl;
    indent(ofs, 2) << "object().addPropertyValue(\"" << _decorated.rdfname() << "\", " <<
        (!rdfCppType.second.undefined ?
         "autordf::PropertyValue().set<autordf::cvt::RdfTypeEnum::" + cvt::rdfTypeEnumString(rdfType) + ">(value), " :
         "value, ") <<
        orderedBoolValue() << ");" << std::endl;
    indent(ofs, 2) << "return *this;" << std::endl;
    indent(ofs, 1) << "}" << std::endl;
}

void DataProperty::generateRemover(std::ostream& ofs, const Klass& onClass) const {
    std::string methodName = "remove" + _decorated.prettyIRIName(true);
    generatePropertyComment(ofs, onClass, methodName, 1,
                    "Remove a value for this property.\n"
                            "@param value to remove for this property.\n"
                            "@throw PropertyNotFound if propertyIRI has not obj as value\n");
    std::pair<cvt::RdfTypeEnum, CppType> rdfCppType = getRdfCppTypes(onClass);

    std::string currentClassName = "I" + onClass.decorated().prettyIRIName();

    cvt::RdfTypeEnum rdfType = rdfCppType.first;
    std::string cppType = rdfCppType.second.oneType;
    indent(ofs, 1) << currentClassName << "& " << methodName << "(const " << cppType << "& value) {" << std::endl;
    indent(ofs, 2) << "object().removePropertyValue(\"" << _decorated.rdfname() << "\", " <<
        (!rdfCppType.second.undefined ?
         "autordf::PropertyValue().set<autordf::cvt::RdfTypeEnum::" + cvt::rdfTypeEnumString(rdfType) + ">(value)); " :
         "value);") << std::endl;
    indent(ofs, 2) << "return *this;" << std::endl;
    indent(ofs, 1) << "}" << std::endl;
}

std::string DataProperty::orderedBoolValue() const {
    return _decorated.ordered() ? "true" : "false";
}

}
}
