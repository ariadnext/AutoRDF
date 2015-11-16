#ifndef AUTORDF_CVT_CVT_H
#define AUTORDF_CVT_CVT_H

#include <string>
#include <stdexcept>
#include <sstream>

#include <boost/lexical_cast.hpp>
#include <boost/date_time.hpp>

#include <autordf/cvt/RdfTypeEnum.h>
#include <autordf/Exception.h>

namespace autordf {
namespace cvt {

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

template<typename CppType, RdfTypeEnum rdfType> class toCpp;

template<typename CppType> CppType toCppGeneric(const std::string& rawValue) {
    try {
        return boost::lexical_cast<CppType>(trim(rawValue));
    }
    catch (const std::exception& e) {
        throw DataConvertionFailure("During convertion of " + rawValue + ": " + e.what());
    }
}

inline boost::posix_time::ptime toCppDateTime(const std::string& rawValue) {
    try {
        std::istringstream ss(trim(rawValue));
        ss.exceptions(std::ios_base::failbit);
        boost::posix_time::time_input_facet* input_facet = new boost::posix_time::time_input_facet;
        input_facet->format("%Y-%m-%dT%H:%M:%S%F%Q");
        ss.imbue(std::locale(std::locale::classic(), input_facet));
        boost::local_time::local_date_time result(boost::date_time::not_a_date_time);
        ss >> result;
        return result.utc_time();
    } catch (const std::exception& e) {
        throw DataConvertionFailure("During convertion of " + rawValue + ": " + e.what());
    }
};

#define GENERIC_TOCPP(cppType, rdfType) \
template<> class toCpp<cppType, RdfTypeEnum::rdfType> { \
public: \
    static cppType val(const std::string& rawValue) { return toCppGeneric<cppType>(rawValue); } \
}; \

// xs:string
template<RdfTypeEnum rdfType> class toCpp<std::string, rdfType> {
public:
    static std::string val(const std::string& rawValue) {
        return rawValue;
    }
};

// xsd:integer
GENERIC_TOCPP(long long int, xsd_integer)

// xsd:decimal
GENERIC_TOCPP(double, xsd_decimal)

// xsd:float
GENERIC_TOCPP(float, xsd_float)

// xsd:double
GENERIC_TOCPP(double, xsd_double)

// xsd_boolean
template<typename CppType> class toCpp<CppType, RdfTypeEnum::xsd_boolean> {
public:
    static CppType val(const std::string& rawValue) {
        std::string left = trim(rawValue);
        if ( left == "true" || left == "1") {
            return true;
        } else if ( left == "false" || left == "0") {
            return false;
        } else {
            throw DataConvertionFailure("Invalid value '" + left + "' for boolean");
        }
    }
};

// xsd:dateTime
template<> class toCpp<boost::posix_time::ptime, RdfTypeEnum::xsd_dateTime> {
public:
    static boost::posix_time::ptime val(const std::string& rawValue) {
        return toCppDateTime(rawValue);
    }
};

// xsd:dateTimeStamp
template<> class toCpp<boost::posix_time::ptime, RdfTypeEnum::xsd_dateTimeStamp> {
public:
    static boost::posix_time::ptime val(const std::string& rawValue) {
        return toCppDateTime(rawValue);
    }
};

// xsd:nonNegativeInteger
GENERIC_TOCPP(long long unsigned int, xsd_nonNegativeInteger)

// xsd:positiveInteger
GENERIC_TOCPP(long long unsigned int, xsd_positiveInteger)

// xsd:nonPositiveInteger
GENERIC_TOCPP(long long int, xsd_nonPositiveInteger)

// xsd:negativeInteger
GENERIC_TOCPP(long long int, xsd_negativeInteger)

// xsd:long
GENERIC_TOCPP(long long int, xsd_long)

// xsd:unsignedLong
GENERIC_TOCPP(long long unsigned int, xsd_unsignedLong)

// xsd:int
GENERIC_TOCPP(long int, xsd_int)

// xsd:unsignedInt
GENERIC_TOCPP(long unsigned int, xsd_unsignedInt)

// xsd:short
GENERIC_TOCPP(short, xsd_short)

// xsd:unsignedShort
GENERIC_TOCPP(unsigned short, xsd_unsignedShort)

// xsd:byte
template<> class toCpp<char, RdfTypeEnum::xsd_byte> {
public:
    static char val(const std::string& rawValue) {
        return boost::lexical_cast<short>(trim(rawValue));
    }
};

// xsd:unsignedByte
template<> class toCpp<unsigned char, RdfTypeEnum::xsd_unsignedByte> {
public:
    static unsigned char val(const std::string& rawValue) {
        return boost::lexical_cast<unsigned short>(trim(rawValue));
    }
};

template<typename CppType, RdfTypeEnum rdfType> class toRdf;

template<RdfTypeEnum rdfType, typename CppType> std::string toRdfGeneric(const CppType& cppValue) {
    std::ostringstream ss;
    ss << cppValue;
    return ss.str();
};

#define GENERIC_TORDF(cppType, rdfType) \
template<> class toRdf<cppType, RdfTypeEnum::rdfType> { \
public: \
    static std::string val(const cppType& cppValue) { return toRdfGeneric<RdfTypeEnum::rdfType>(cppValue); } \
};

// xsd:string
GENERIC_TORDF(std::string, xsd_string)

// xsd:integer
GENERIC_TORDF(long long int, xsd_integer)

// xsd:decimal
GENERIC_TORDF(double, xsd_decimal)

// xsd:float
GENERIC_TORDF(float, xsd_float)

// xsd:double
GENERIC_TORDF(double, xsd_double)

// xsd:boolean
template<typename CppType> class toRdf<CppType, RdfTypeEnum::xsd_boolean>  {
public:
    static std::string val(const CppType& cppValue) {
        return (cppValue ? "true" : "false");
    }
};

inline std::string toRdfDateTime(const boost::posix_time::ptime& time) {
    std::ostringstream ss;
    ss.exceptions(std::ios_base::failbit);
    boost::posix_time::time_facet* facet = new boost::posix_time::time_facet;
    // ISO Extended with Z as forced timezone
    facet->format("%Y-%m-%dT%H:%M:%S%FZ");
    ss.imbue(std::locale(std::locale::classic(), facet));
    ss << time;
    return ss.str();
}

// xsd:dateTime
template<> class toRdf<boost::posix_time::ptime, RdfTypeEnum::xsd_dateTime> {
public:
    static std::string val(const boost::posix_time::ptime& time) {
        return toRdfDateTime(time);
    }
};

// xsd:dateTimeStamp
template<> class toRdf<boost::posix_time::ptime, RdfTypeEnum::xsd_dateTimeStamp> {
public:
    static std::string val(const boost::posix_time::ptime& time) {
        return toRdfDateTime(time);
    }
};

// xsd:nonNegativeInteger
GENERIC_TORDF(long long unsigned int, xsd_nonNegativeInteger)

// xsd:positiveInteger
GENERIC_TORDF(long long unsigned int, xsd_positiveInteger)

// xsd:nonPositiveInteger
GENERIC_TORDF(long long int, xsd_nonPositiveInteger)

// xsd:negativeInteger
GENERIC_TORDF(long long int, xsd_negativeInteger)

// xsd:long
GENERIC_TORDF(long long int, xsd_long)

// xsd:unsignedLong
GENERIC_TORDF(long long unsigned int, xsd_unsignedLong)

// xsd:int
GENERIC_TORDF(long int, xsd_int)

// xsd:unsignedInt
GENERIC_TORDF(long unsigned int, xsd_unsignedInt)

// xsd:short
GENERIC_TORDF(short, xsd_short)

// xsd:unsignedShort
GENERIC_TORDF(unsigned short, xsd_unsignedShort)

// xsd:byte
template<> class toRdf<char, RdfTypeEnum::xsd_byte>  {
public:
    static std::string val(char cppValue) {
        std::ostringstream ss;
        ss << static_cast<short>(cppValue);
        return ss.str();
    }
};

// xsd:unsignedByte
template<> class toRdf<unsigned char, RdfTypeEnum::xsd_unsignedByte>  {
public:
    static std::string val(unsigned char cppValue) {
        std::ostringstream ss;
        ss << static_cast<unsigned short>(cppValue);
        return ss.str();
    }
};
}
}
#endif //AUTORDF_CVT_CVT_H
