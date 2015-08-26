#include <gtest/gtest.h>

#include <boost/filesystem.hpp>

#include "autordf/cvt/Cvt.h"

using namespace autordf::cvt;

TEST(_04_cvt, xsd_to_cpp) {
    ASSERT_EQ("date of birth", (toCpp<std::string, RdfTypeEnum::xsd_string>::val("\"date of birth\"^^xsd:string")));
    ASSERT_EQ(-210, (toCpp<long long int, RdfTypeEnum::xsd_integer>::val("\"-210\"^^xsd:integer")));
    ASSERT_EQ(210, (toCpp<long long unsigned int, RdfTypeEnum::xsd_nonNegativeInteger>::val("\"210\"^^xsd:nonNegativeInteger")));
    ASSERT_EQ(210, (toCpp<long long unsigned int, RdfTypeEnum::xsd_positiveInteger>::val("\"210\"^^xsd:positiveInteger")));
    ASSERT_EQ(-210, (toCpp<long long int, RdfTypeEnum::xsd_negativeInteger>::val("\"-210\"^^xsd:negativeInteger")));
    ASSERT_EQ(-210, (toCpp<long long int, RdfTypeEnum::xsd_nonPositiveInteger>::val("\"-210\"^^xsd:nonPositiveInteger")));
    ASSERT_EQ(210, (toCpp<long long unsigned int, RdfTypeEnum::xsd_unsignedLong>::val("\"210\"^^xsd:unsignedLong")));
    ASSERT_EQ(-210, (toCpp<long long int, RdfTypeEnum::xsd_long>::val("\"-210\"^^xsd:long")));
    ASSERT_EQ(210, (toCpp<long unsigned int, RdfTypeEnum::xsd_unsignedInt>::val("\"210\"^^xsd:unsignedInt")));
    ASSERT_EQ(-210, (toCpp<long int, RdfTypeEnum::xsd_int>::val("\"-210\"^^xsd:int")));
    ASSERT_EQ(210, (toCpp<unsigned short, RdfTypeEnum::xsd_unsignedShort>::val("\"210\"^^xsd:unsignedShort")));
    ASSERT_EQ(-210, (toCpp<short, RdfTypeEnum::xsd_short>::val("\"-210\"^^xsd:short")));
    ASSERT_EQ(127, (toCpp<unsigned char, RdfTypeEnum::xsd_unsignedByte>::val("\"127\"^^xsd:unsignedByte")));
    ASSERT_EQ(-127, (toCpp<char, RdfTypeEnum::xsd_byte>::val("\"-127\"^^xsd:byte")));
    ASSERT_EQ(true, (toCpp<bool, RdfTypeEnum::xsd_boolean>::val("\"true\"^^xsd:boolean")));
    ASSERT_EQ(true, (toCpp<bool, RdfTypeEnum::xsd_boolean>::val("\"1\"^^xsd:boolean")));
    ASSERT_EQ(false, (toCpp<bool, RdfTypeEnum::xsd_boolean>::val("\"0\"^^xsd:boolean")));
    ASSERT_EQ(0.5, (toCpp<double, RdfTypeEnum::xsd_decimal>::val("\"0.5\"^^xsd:decimal")));
    ASSERT_EQ(0.5, (toCpp<float, RdfTypeEnum::xsd_float>::val("\"0.5\"^^xsd:float")));
    ASSERT_EQ(0.5, (toCpp<double, RdfTypeEnum::xsd_double>::val("\"0.5\"^^xsd:double")));
    ASSERT_EQ("1977-Apr-22 06:00:00", boost::posix_time::to_simple_string(toCpp<boost::posix_time::ptime, RdfTypeEnum::xsd_dateTime>::val("\"1977-04-22T06:00:00\"^^xsd:dateTime")));
    ASSERT_EQ("1977-Apr-22 06:00:00", boost::posix_time::to_simple_string(toCpp<boost::posix_time::ptime, RdfTypeEnum::xsd_dateTimeStamp>::val("\"1977-04-22T06:00:00Z\"^^xsd:dateTimeStamp")));

    //Checking trimming
    ASSERT_EQ("date of birth  ", (toCpp<std::string, RdfTypeEnum::xsd_string>::val("\"date of birth  \"^^xsd:string")));
    ASSERT_EQ(true, (toCpp<bool, RdfTypeEnum::xsd_boolean>::val("\"1 \"^^xsd:boolean")));
    ASSERT_EQ(false, (toCpp<bool, RdfTypeEnum::xsd_boolean>::val("\" 0\"^^xsd:boolean")));
    ASSERT_EQ(210, (toCpp<long long, RdfTypeEnum::xsd_integer>::val("\"\t210\n\"^^xsd:integer")));
}

TEST(_04_cvt, cpp_to_xsd) {
    ASSERT_EQ("\"date of birth\"^^xsd:string", (toRdf<std::string, RdfTypeEnum::xsd_string>::val("date of birth")));
    ASSERT_EQ("\"-210\"^^xsd:integer", (toRdf<long long int, RdfTypeEnum::xsd_integer>::val(-210)));
    ASSERT_EQ("\"210\"^^xsd:nonNegativeInteger", (toRdf<long long unsigned int, RdfTypeEnum::xsd_nonNegativeInteger>::val(210)));
    ASSERT_EQ("\"210\"^^xsd:positiveInteger", (toRdf<long long unsigned int, RdfTypeEnum::xsd_positiveInteger>::val(210)));
    ASSERT_EQ("\"-210\"^^xsd:negativeInteger", (toRdf<long long int, RdfTypeEnum::xsd_negativeInteger>::val(-210)));
    ASSERT_EQ("\"-210\"^^xsd:nonPositiveInteger", (toRdf<long long int, RdfTypeEnum::xsd_nonPositiveInteger>::val(-210)));
    ASSERT_EQ("\"210\"^^xsd:unsignedLong", (toRdf<long long unsigned int, RdfTypeEnum::xsd_unsignedLong>::val(210)));
    ASSERT_EQ("\"-210\"^^xsd:long", (toRdf<long long int, RdfTypeEnum::xsd_long>::val(-210)));
    ASSERT_EQ("\"210\"^^xsd:unsignedInt", (toRdf<long unsigned int, RdfTypeEnum::xsd_unsignedInt>::val(210)));
    ASSERT_EQ("\"-210\"^^xsd:int", (toRdf<long int, RdfTypeEnum::xsd_int>::val(-210)));
    ASSERT_EQ("\"210\"^^xsd:unsignedShort", (toRdf<unsigned short, RdfTypeEnum::xsd_unsignedShort>::val(210)));
    ASSERT_EQ("\"-210\"^^xsd:short", (toRdf<short, RdfTypeEnum::xsd_short>::val(-210)));
    ASSERT_EQ("\"127\"^^xsd:unsignedByte", (toRdf<unsigned char, RdfTypeEnum::xsd_unsignedByte>::val(127)));
    ASSERT_EQ("\"-127\"^^xsd:byte", (toRdf<char, RdfTypeEnum::xsd_byte>::val(-127)));
    ASSERT_EQ("\"true\"^^xsd:boolean", (toRdf<bool, RdfTypeEnum::xsd_boolean>::val(true)));
    // That works too
    ASSERT_EQ("\"true\"^^xsd:boolean", (toRdf<int, RdfTypeEnum::xsd_boolean>::val(5)));
    // That works too
    ASSERT_EQ("\"false\"^^xsd:boolean", (toRdf<float, RdfTypeEnum::xsd_boolean>::val(0.0)));

    ASSERT_EQ("\"0.5\"^^xsd:decimal", (toRdf<double, RdfTypeEnum::xsd_decimal>::val(0.5)));
    ASSERT_EQ("\"0.5\"^^xsd:float", (toRdf<float, RdfTypeEnum::xsd_float>::val(0.5)));
    ASSERT_EQ("\"0.5\"^^xsd:double", (toRdf<double, RdfTypeEnum::xsd_double>::val(0.5)));
    ASSERT_EQ("\"2002-01-20T23:59:59.001000Z\"^^xsd:dateTime", (toRdf<boost::posix_time::ptime, RdfTypeEnum::xsd_dateTime>::val(boost::posix_time::time_from_string("2002-01-20 23:59:59.001"))));
    ASSERT_EQ("\"2002-01-20T23:59:59.001000Z\"^^xsd:dateTimeStamp", (toRdf<boost::posix_time::ptime, RdfTypeEnum::xsd_dateTimeStamp>::val(boost::posix_time::time_from_string("2002-01-20 23:59:59.001"))));
}

// This is cuurently noot supported by boost
TEST(_04_cvt, DISABLED_boostbugs) {
    // This example taken from http://www.w3.org/TR/xmlschema-2/#dateTime fails :-(
    ASSERT_EQ("2002-Oct-10 17:00:00", boost::posix_time::to_simple_string(toCpp<boost::posix_time::ptime, RdfTypeEnum::xsd_dateTime>::val("\"2002-10-10T12:00:00-05:00\"^^xsd:dateTime")));
}