#ifndef AUTORDF_CVT_CVT_H
#define AUTORDF_CVT_CVT_H

#include <string>
#include <stdexcept>
#include <sstream>

#include <boost/lexical_cast.hpp>
#include <boost/date_time.hpp>

namespace autordf {
namespace cvt {

#define CVT_TYPES_DEF(X) \
    X(xsd_string) \
    X(xsd_boolean) \
    X(xsd_decimal) \
    X(xsd_float) \
    X(xsd_double) \
    X(xsd_dateTime) \
    X(xsd_int)

enum class RdfTypeEnum {
#define X(a) a,
    CVT_TYPES_DEF(X)
#undef X
};

inline std::string trim(const std::string &s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && isspace(*it))
        it++;

    std::string::const_reverse_iterator rit = s.rbegin();
    while (rit.base() != it && isspace(*rit))
        rit++;

    return std::string(it, rit.base());
}

inline std::string rdfTypeEnumString(RdfTypeEnum enumVal) {
    static const std::string RAWVALS[] = {
#define X(a) #a,
    CVT_TYPES_DEF(X)
#undef X
    };
    std::string val = RAWVALS[static_cast<int>(enumVal)];
    for ( char& c : val) {
        if ( c == '_' ) {
            c = ':';
        }
    }
    return val;
}

inline std::string getLeft(const std::string& rawValue) {
    auto pos = rawValue.find('^');
    if ( pos != std::string::npos ) {
        return rawValue.substr(1, pos - 2);
    } else {
        throw std::runtime_error("Unable to convert value as ^^ was not found in: " + rawValue);
    }
}

template<typename CppType, RdfTypeEnum rdfType> class toCpp {
public:
    static CppType val(const std::string& rawValue) {
        return  boost::lexical_cast<CppType>(trim(getLeft(rawValue)));
    }
};

template<RdfTypeEnum rdfType> class toCpp<std::string, rdfType> {
public:
    static std::string val(const std::string& rawValue) {
        return getLeft(rawValue);
    }
};

template<typename CppType> class toCpp<CppType, RdfTypeEnum::xsd_boolean> {
public:
    static CppType val(const std::string& rawValue) {
        std::string left = trim(getLeft(rawValue));
        if ( left == "true" || left == "1") {
            return true;
        } else if ( left == "false" || left == "0") {
            return false;
        } else {
            throw std::runtime_error("Invalid value '" + left + "' for boolean");
        }
    }
};

template<> class toCpp<boost::posix_time::ptime, RdfTypeEnum::xsd_dateTime> {
public:
    static boost::posix_time::ptime val(const std::string& rawValue) {
        std::stringstream ss(trim(getLeft(rawValue)));
        ss.exceptions(std::ios_base::failbit);
        boost::local_time::local_time_input_facet* input_facet = new boost::local_time::local_time_input_facet;
        input_facet->set_iso_extended_format();
        ss.imbue(std::locale(std::locale::classic(), input_facet));
        boost::local_time::local_date_time result(boost::date_time::not_a_date_time);
        ss >> result;
        return result.utc_time();
    }
};

template<typename CppType, RdfTypeEnum rdfType> class toRdf {
public:
    static std::string val(const CppType& cppValue) {
        std::ostringstream ss;
        ss << "\"" << cppValue << "\"^^" << rdfTypeEnumString(rdfType);
        return ss.str();
    }
};

template<typename CppType> class toRdf<CppType, RdfTypeEnum::xsd_boolean>  {
public:
    static std::string val(const CppType& cppValue) {
        return  std::string("\"") + (cppValue ? "true" : "false") + "\"^^" + rdfTypeEnumString(RdfTypeEnum::xsd_boolean);
    }
};

template<> class toRdf<boost::posix_time::ptime, RdfTypeEnum::xsd_dateTime> {
public:
    static std::string val(const boost::posix_time::ptime& time) {
        return  std::string("\"") + boost::posix_time::to_iso_extended_string(time) + "Z\"^^" + rdfTypeEnumString(RdfTypeEnum::xsd_dateTime);
    }
};

}
}
#endif //AUTORDF_CVT_CVT_H
