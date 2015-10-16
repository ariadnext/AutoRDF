#include "DataProperty.h"

#include <autordf/cvt/Cvt.h>

#include "Klass.h"
#include "Utils.h"

namespace autordf {
namespace codegen {

std::tuple<const char *, cvt::RdfTypeEnum, const char *> rdf2CppTypeMapping[] = {
//FIXME: handle PlainLiteral, XMLLiteral, Literal, real, rational
        std::make_tuple("http://www.w3.org/2001/XMLSchema#string",             cvt::RdfTypeEnum::xsd_string,             "std::string"),
        std::make_tuple("http://www.w3.org/2001/XMLSchema#boolean",            cvt::RdfTypeEnum::xsd_boolean,            "bool"),
        std::make_tuple("http://www.w3.org/2001/XMLSchema#decimal",            cvt::RdfTypeEnum::xsd_decimal,            "double"),
        std::make_tuple("http://www.w3.org/2001/XMLSchema#float",              cvt::RdfTypeEnum::xsd_float,              "float"),
        std::make_tuple("http://www.w3.org/2001/XMLSchema#double",             cvt::RdfTypeEnum::xsd_double,             "double"),
        std::make_tuple("http://www.w3.org/2001/XMLSchema#dateTime",           cvt::RdfTypeEnum::xsd_dateTime,           "boost::posix_time::ptime"),
        std::make_tuple("http://www.w3.org/2001/XMLSchema#integer",            cvt::RdfTypeEnum::xsd_integer,            "long long int"),
        std::make_tuple("http://www.w3.org/2001/XMLSchema#dateTimeStamp",      cvt::RdfTypeEnum::xsd_dateTimeStamp,      "boost::posix_time::ptime"),
        std::make_tuple("http://www.w3.org/2001/XMLSchema#nonNegativeInteger", cvt::RdfTypeEnum::xsd_nonNegativeInteger, "long long unsigned int"),
        std::make_tuple("http://www.w3.org/2001/XMLSchema#positiveInteger",    cvt::RdfTypeEnum::xsd_positiveInteger,    "long long unsigned int"),
        std::make_tuple("http://www.w3.org/2001/XMLSchema#nonPositiveInteger", cvt::RdfTypeEnum::xsd_nonPositiveInteger, "long long int"),
        std::make_tuple("http://www.w3.org/2001/XMLSchema#negativeInteger",    cvt::RdfTypeEnum::xsd_negativeInteger,    "long long int"),
        std::make_tuple("http://www.w3.org/2001/XMLSchema#long",               cvt::RdfTypeEnum::xsd_long,               "long long int"),
        std::make_tuple("http://www.w3.org/2001/XMLSchema#unsignedLong",       cvt::RdfTypeEnum::xsd_unsignedLong,       "unsigned long long int"),
        std::make_tuple("http://www.w3.org/2001/XMLSchema#int",                cvt::RdfTypeEnum::xsd_int,                "long int"),
        std::make_tuple("http://www.w3.org/2001/XMLSchema#unsignedInt",        cvt::RdfTypeEnum::xsd_unsignedInt,        "long unsigned int"),
        std::make_tuple("http://www.w3.org/2001/XMLSchema#short",              cvt::RdfTypeEnum::xsd_short,              "short"),
        std::make_tuple("http://www.w3.org/2001/XMLSchema#unsignedShort",      cvt::RdfTypeEnum::xsd_unsignedShort,      "unsigned short"),
        std::make_tuple("http://www.w3.org/2001/XMLSchema#byte",               cvt::RdfTypeEnum::xsd_byte,               "char"),
        std::make_tuple("http://www.w3.org/2001/XMLSchema#unsignedByte",       cvt::RdfTypeEnum::xsd_unsignedByte,       "unsigned char")
};

void DataProperty::generateDeclaration(std::ostream& ofs, const Klass& onClass) const {
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
}

void DataProperty::generateDefinition(std::ostream& ofs, const Klass& onClass) const {
    int index = range2CvtArrayIndex(onClass);
    if ( index >= 0 ) {
        const char *cppType = std::get<2>(rdf2CppTypeMapping[index]);
        cvt::RdfTypeEnum rdfType = std::get<1>(rdf2CppTypeMapping[index]);

        std::string currentClassName = "I" + onClass.decorated().prettyIRIName();

        if (_decorated.maxCardinality(onClass.decorated()) <= 1) {
            if (_decorated.minCardinality(onClass.decorated()) > 0) {
                // Nothing
            } else {
                ofs << "std::shared_ptr<" << cppType << "> " << currentClassName << "::" << _decorated.prettyIRIName() << "Optional() const {" << std::endl;
                indent(ofs, 1) << "auto ptrval = object().getOptionalPropertyValue(\"" << _decorated.rdfname() << "\");" << std::endl;
                indent(ofs, 1) << "return (ptrval ? std::shared_ptr<" << cppType << ">(new " << cppType <<
                "(ptrval->get<autordf::cvt::RdfTypeEnum::" << cvt::rdfTypeEnumString(rdfType) << ", " << cppType << ">())) : nullptr);" << std::endl;
                ofs << "}" << std::endl;
            }
        }
        if (_decorated.maxCardinality(onClass.decorated()) > 1) {
            ofs << "std::list<" << cppType << "> " << currentClassName << "::" << _decorated.prettyIRIName() <<
            "List() const {" << std::endl;
            indent(ofs, 1) << "return object().getValueListImpl<autordf::cvt::RdfTypeEnum::" <<
            cvt::rdfTypeEnumString(rdfType) << ", " << cppType << ">(\"" << _decorated.rdfname() << "\");" << std::endl;
            ofs << "}" << std::endl;
            ofs << std::endl;
            ofs << "void " << currentClassName << "::set" << _decorated.prettyIRIName(true) << "(const std::list<" << cppType << ">& values) {" << std::endl;
            indent(ofs, 1) <<     "object().setValueListImpl<autordf::cvt::RdfTypeEnum::" <<
            cvt::rdfTypeEnumString(rdfType) << ">(\"" << _decorated.rdfname() << "\", values);" << std::endl;
            ofs << "}" << std::endl;
        }
    }
}

int DataProperty::range2CvtArrayIndex(const Klass& onClass) const {
    std::string effectiveRange = _decorated.range(&onClass.decorated());
    if ( !effectiveRange.empty() ) {
        for ( unsigned int i = 0; i < (sizeof(rdf2CppTypeMapping) / sizeof(rdf2CppTypeMapping[0])); ++i ) {
            if ( std::get<0>(rdf2CppTypeMapping[i]) == effectiveRange ) {
                return i;
            }
        }
        std::cerr << effectiveRange << " type not supported, we will default to raw value for property " << _decorated.rdfname() << std::endl;
    }
    return -1;
}


void DataProperty::generateGetterForOneMandatory(std::ostream& ofs, const Klass& onClass) const {
    generateComment(ofs, 1,
                    "@return the mandatory value for this property.\n"
                            "@throw PropertyNotFound if value is not set in database\n"
                            "@throw DuplicateProperty if database contains more than one value");
    int index = range2CvtArrayIndex(onClass);
    if ( index >= 0 ) {
        const char *cppType = std::get<2>(rdf2CppTypeMapping[index]);
        cvt::RdfTypeEnum rdfType = std::get<1>(rdf2CppTypeMapping[index]);
        indent(ofs, 1) << cppType << " " << _decorated.prettyIRIName() << "() const {" << std::endl;
        indent(ofs, 2) << "return object().getPropertyValue(\"" << _decorated.rdfname() << "\").get<autordf::cvt::RdfTypeEnum::" << cvt::rdfTypeEnumString(rdfType) << ", " << cppType<< ">();" << std::endl;
        indent(ofs, 1) << "}" << std::endl;
    } else {
        indent(ofs, 1) << "autordf::PropertyValue " << _decorated.prettyIRIName() << "() const {" << std::endl;
        indent(ofs, 2) << "return object().getPropertyValue(\"" << _decorated.rdfname() << "\");" << std::endl;
        indent(ofs, 1) << "}" << std::endl;
    }
}

void DataProperty::generateSetterForOne(std::ostream& ofs, const Klass& onClass) const {
    generateComment(ofs, 1,
                    "Sets the mandatory value for this property.\n"
                            "@param value value to set for this property, removing all other values");
    int index = range2CvtArrayIndex(onClass);
    if (index >= 0) {
        const char *cppType = std::get<2>(rdf2CppTypeMapping[index]);
        cvt::RdfTypeEnum rdfType = std::get<1>(rdf2CppTypeMapping[index]);
        indent(ofs, 1) << "void set" << _decorated.prettyIRIName(true) << "(const " << cppType << "& value) {" << std::endl;
        indent(ofs, 2) << "return object().setPropertyValue(\"" << _decorated.rdfname() <<
            "\", autordf::PropertyValue().set<autordf::cvt::RdfTypeEnum::" << cvt::rdfTypeEnumString(rdfType) <<
            ">(value));" << std::endl;
        indent(ofs, 1) << "}" << std::endl;
    } else {
        indent(ofs, 1) << "void set" << _decorated.prettyIRIName(true) << "(const autordf::PropertyValue& value) {" << std::endl;
        indent(ofs, 2) << "object().setPropertyValue(\"" << _decorated.rdfname() << "\", value);" << std::endl;
        indent(ofs, 1) << "}" << std::endl;
    }
}

void DataProperty::generateGetterForOneOptional(std::ostream& ofs, const Klass& onClass) const {
    generateComment(ofs, 1,
                    "@return the valueif it is set, or nullptr if it is not set.\n"
                            "@throw DuplicateProperty if database contains more than one value");
    int index = range2CvtArrayIndex(onClass);
    if ( index >= 0 ) {
        const char *cppType = std::get<2>(rdf2CppTypeMapping[index]);
        indent(ofs, 1) << "std::shared_ptr<" << cppType << "> " << _decorated.prettyIRIName() << "Optional() const;" << std::endl;
    } else {
        indent(ofs, 1) << "std::shared_ptr<autordf::PropertyValue> " << _decorated.prettyIRIName() << "Optional() const {" << std::endl;
        indent(ofs, 2) << "return object().getOptionalPropertyValue(\"" << _decorated.rdfname() << "\");" << std::endl;
        indent(ofs, 1) << "}" << std::endl;
    }
}

void DataProperty::generateGetterForMany(std::ostream& ofs, const Klass& onClass) const {
    generateComment(ofs, 1,
                    "@return the list of values.  List can be empty if not values are set in database");
    int index = range2CvtArrayIndex(onClass);
    if ( index >= 0 ) {
        const char *cppType = std::get<2>(rdf2CppTypeMapping[index]);
        indent(ofs, 1) << "std::list<" << cppType << "> " << _decorated.prettyIRIName() << "List() const;" << std::endl;
    } else {
        indent(ofs, 1) << "std::list<autordf::PropertyValue> " << _decorated.prettyIRIName() << "List() const {" << std::endl;
        indent(ofs, 2) <<     "return object().getPropertyValueList(\"" << _decorated.rdfname() << "\");" << std::endl;
        indent(ofs, 1) << "}" << std::endl;
    }
}

void DataProperty::generateSetterForMany(std::ostream& ofs, const Klass& onClass) const {
    generateComment(ofs, 1,
                    "Sets property to list of values \n@param values the list of values");
    int index = range2CvtArrayIndex(onClass);
    if ( index >= 0 ) {
        const char *cppType = std::get<2>(rdf2CppTypeMapping[index]);
        indent(ofs, 1) << "void set" << _decorated.prettyIRIName(true) << "(const std::list<" << cppType << "> " << "& values);" << std::endl;
    } else {
        indent(ofs, 1) << "void set" << _decorated.prettyIRIName(true) << "(const std::list<autordf::PropertyValue>& values) {" << std::endl;
        indent(ofs, 2) <<     "object().setPropertyValueList(\"" << _decorated.rdfname() << "\", values);" << std::endl;
        indent(ofs, 1) << "}" << std::endl;
    }
}
}
}
