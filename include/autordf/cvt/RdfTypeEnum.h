#ifndef AUTORDF_CVT_RDFTYPENUM_H
#define AUTORDF_CVT_RDFTYPENUM_H

#include <ostream>

namespace autordf {
namespace cvt {

#define CVT_TYPES_DEF(X) \
    X(xsd_string) \
    X(xsd_boolean) \
    X(xsd_decimal) \
    X(xsd_float) \
    X(xsd_double) \
    X(xsd_dateTime) \
    X(xsd_integer) \
    X(xsd_dateTimeStamp) \
    X(xsd_nonNegativeInteger) \
    X(xsd_positiveInteger) \
    X(xsd_nonPositiveInteger) \
    X(xsd_negativeInteger) \
    X(xsd_long) \
    X(xsd_unsignedLong) \
    X(xsd_int) \
    X(xsd_unsignedInt) \
    X(xsd_short) \
    X(xsd_unsignedShort) \
    X(xsd_byte) \
    X(xsd_unsignedByte)

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
#define X(a) a,
   CVT_TYPES_DEF(X)
#undef X
};

inline std::string rdfTypeEnumString(RdfTypeEnum enumVal) {
    static const std::string RAWVALS[] = {
#define X(a) #a,
            CVT_TYPES_DEF(X)
#undef X
    };
    return RAWVALS[static_cast<int>(enumVal)];
}

inline std::string rdfTypeEnumXMLString(RdfTypeEnum enumVal) {
    std::string val = rdfTypeEnumString(enumVal);
    for ( char& c : val) {
        if ( c == '_' ) {
            c = ':';
        }
    }
    return val;
}

inline std::ostream& operator<<(std::ostream& os, RdfTypeEnum enumVal) {
    os << rdfTypeEnumXMLString(enumVal);
    return os;
}

}
}
#endif //AUTORDF_CVT_RDFTYPENUM_H
