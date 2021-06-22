#include <autordf/cvt/RdfTypeEnum.h>

#include <ostream>

namespace autordf {
namespace cvt {

std::string rdfTypeEnumString(RdfTypeEnum enumVal) {
    static const std::string RAWVALS[] = {
#define X(a, b) #a,
            CVT_TYPES_DEF(X)
#undef X
    };
    return RAWVALS[static_cast<int>(enumVal)];
}

AUTORDF_EXPORT std::map<std::string, RdfTypeEnum> rdfMapType(
        {
#define X(type, uri) std::pair<const std::string, RdfTypeEnum>("http://www.w3.org/2001/XMLSchema#" uri, RdfTypeEnum::type),
        CVT_TYPES_DEF(X)
#undef X
});

std::string rdfTypeEnumXMLString(RdfTypeEnum enumVal) {
    std::string val = rdfTypeEnumString(enumVal);
    for ( char& c : val) {
        if ( c == '_' ) {
            c = ':';
        }
    }
    return val;
}

std::ostream& operator<<(std::ostream& os, RdfTypeEnum enumVal) {
    os << rdfTypeEnumXMLString(enumVal);
    return os;
}

}
}
