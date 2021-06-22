#ifndef AUTORDF_CVT_RDFTYPENUM_H
#define AUTORDF_CVT_RDFTYPENUM_H

#include <iosfwd>
#include <map>
#include <string>

#include <autordf/autordf_export.h>

namespace autordf {
namespace cvt {

/*
 * Xmacro containing RdftypeEnum <-> Rdf uri type :
 * {label, short name, namespace}
 */
#define CVT_TYPES_DEF(X) \
    X(xsd_string,                   "string",              "http://www.w3.org/2001/XMLSchema#") \
    X(xsd_boolean,                  "boolean",             "http://www.w3.org/2001/XMLSchema#") \
    X(xsd_decimal,                  "decimal",             "http://www.w3.org/2001/XMLSchema#") \
    X(xsd_float,                    "float",               "http://www.w3.org/2001/XMLSchema#") \
    X(xsd_double,                   "double",              "http://www.w3.org/2001/XMLSchema#") \
    X(xsd_dateTime,                 "dateTime",            "http://www.w3.org/2001/XMLSchema#") \
    X(xsd_integer,                  "integer",             "http://www.w3.org/2001/XMLSchema#") \
    X(xsd_dateTimeStamp,            "dateTimeStamp",       "http://www.w3.org/2001/XMLSchema#") \
    X(xsd_nonNegativeInteger,       "nonNegativeInteger",  "http://www.w3.org/2001/XMLSchema#") \
    X(xsd_positiveInteger,          "positiveInteger",     "http://www.w3.org/2001/XMLSchema#") \
    X(xsd_nonPositiveInteger,       "nonPositiveInteger",  "http://www.w3.org/2001/XMLSchema#") \
    X(xsd_negativeInteger,          "negativeInteger",     "http://www.w3.org/2001/XMLSchema#") \
    X(xsd_long,                     "long",                "http://www.w3.org/2001/XMLSchema#") \
    X(xsd_unsignedLong,             "unsignedLong",        "http://www.w3.org/2001/XMLSchema#") \
    X(xsd_int,                      "int",                 "http://www.w3.org/2001/XMLSchema#") \
    X(xsd_unsignedInt,              "unsignedInt",         "http://www.w3.org/2001/XMLSchema#") \
    X(xsd_short,                    "short",               "http://www.w3.org/2001/XMLSchema#") \
    X(xsd_unsignedShort,            "unsignedShort",       "http://www.w3.org/2001/XMLSchema#") \
    X(xsd_byte,                     "byte",                "http://www.w3.org/2001/XMLSchema#") \
    X(xsd_unsignedByte,             "unsignedByte",        "http://www.w3.org/2001/XMLSchema#") \
    X(rdf_langString,               "langString",          "http://www.w3.org/1999/02/22-rdf-syntax-ns#")

// Integer types are a pain. Mapping from/to C++ <--> XML Schema is done using info from
// - http://en.cppreference.com/w/cpp/language/types
// and
// - http://www.w3.org/TR/xmlschema-2
//
// 1 - We make sure that data types association between both world are at least big enough to be sure that
// any data read out there will fit in the C++ type, except for xsd:decimal and xsd:*integer, where we merely rely on an approximation
// We approximate xsd:decimal with double and xsd:*integer with long long.
// 2 - However it is responsibility of the user to make sure
// that the data he puts in his C++ variables will fit in the corresponding XSD types

enum class RdfTypeEnum {
#define X(a, b, c) a,
   CVT_TYPES_DEF(X)
#undef X
};

/**
 * Type IRI ( e.g. "http://www.w3.org/2001/XMLSchema#decimal ) to enum (e.g RdfTypeEnum::xsd_decimal)
 */
AUTORDF_EXPORT extern std::map<std::string, RdfTypeEnum> rdfMapType;

/**
 * Returns string rep of enum e.g. "xsd_decimal"
 */
AUTORDF_EXPORT std::string rdfTypeEnumString(RdfTypeEnum enumVal);

/**
 * Returns string rep of enum e.g. "xsd:decimal"
 */
AUTORDF_EXPORT std::string rdfTypeEnumXMLString(RdfTypeEnum enumVal);

std::string rdfTypeIri(RdfTypeEnum enumVal);

std::ostream& operator<<(std::ostream& os, RdfTypeEnum enumVal);

}
}
#endif //AUTORDF_CVT_RDFTYPENUM_H
