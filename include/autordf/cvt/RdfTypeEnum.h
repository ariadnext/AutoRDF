#ifndef AUTORDF_CVT_RDFTYPENUM_H
#define AUTORDF_CVT_RDFTYPENUM_H

#include <iosfwd>
#include <map>
#include <string>

#include <autordf/autordf_export.h>

namespace autordf {
namespace cvt {

/*
 * Xmacro containing RdftypeEnum <-> Rdf uri type, you should add : "http://www.w3.org/2001/XMLSchema#" before
 * FIXME : add "http://www.w3.org/2001/XMLSchema#" prefix in the structure
 */
#define CVT_TYPES_DEF(X) \
    X(xsd_string,                   "string") \
    X(xsd_boolean,                  "boolean") \
    X(xsd_decimal,                  "decimal") \
    X(xsd_float,                    "float") \
    X(xsd_double,                   "double") \
    X(xsd_dateTime,                 "dateTime") \
    X(xsd_integer,                  "integer") \
    X(xsd_dateTimeStamp,            "dateTimeStamp") \
    X(xsd_nonNegativeInteger,       "nonNegativeInteger") \
    X(xsd_positiveInteger,          "positiveInteger") \
    X(xsd_nonPositiveInteger,       "nonPositiveInteger") \
    X(xsd_negativeInteger,          "negativeInteger") \
    X(xsd_long,                     "long") \
    X(xsd_unsignedLong,             "unsignedLong") \
    X(xsd_int,                      "int") \
    X(xsd_unsignedInt,              "unsignedInt") \
    X(xsd_short,                    "short") \
    X(xsd_unsignedShort,            "unsignedShort") \
    X(xsd_byte,                     "byte") \
    X(xsd_unsignedByte,             "unsignedByte")

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
#define X(a, b) a,
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
std::string rdfTypeEnumString(RdfTypeEnum enumVal);

/**
 * Returns string rep of enum e.g. "xsd:decimal"
 */
std::string rdfTypeEnumXMLString(RdfTypeEnum enumVal);

std::ostream& operator<<(std::ostream& os, RdfTypeEnum enumVal);

}
}
#endif //AUTORDF_CVT_RDFTYPENUM_H
