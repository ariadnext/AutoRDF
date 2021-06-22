#include <autordf/cvt/RdfTypeEnum.h>

#include <ostream>

namespace autordf {
namespace cvt {

std::string rdfTypeEnumString(RdfTypeEnum enumVal) {
    static const std::string RAWVALS[] = {
#define X(a, b, c) #a,
            CVT_TYPES_DEF(X)
#undef X
    };
    return RAWVALS[static_cast<int>(enumVal)];
}

AUTORDF_EXPORT std::map<std::string, RdfTypeEnum> rdfMapType(
        {
#define X(type, shortName, nameSpace) std::pair<const std::string, RdfTypeEnum>(nameSpace shortName, RdfTypeEnum::type),
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

std::string rdfTypeIri(RdfTypeEnum enumVal) {
    static const std::string RAWVALS[] = {
#define X(a, b, c) c b,
            CVT_TYPES_DEF(X)
#undef X
    };
    return RAWVALS[static_cast<int>(enumVal)];
}

std::ostream& operator<<(std::ostream& os, RdfTypeEnum enumVal) {
    os << rdfTypeEnumXMLString(enumVal);
    return os;
}

}
}
