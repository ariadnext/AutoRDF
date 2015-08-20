#include <gtest/gtest.h>

#include <boost/filesystem.hpp>

#include "autordf/cvt/Cvt.h"

using namespace autordf::cvt;

TEST(_04_cvt, xsd_to_cpp) {
    ASSERT_EQ("date of birth", (toCpp<std::string, RdfTypeEnum::xsd_string>::val("\"date of birth\"^^xsd:string")));
    ASSERT_EQ(210, (toCpp<int, RdfTypeEnum::xsd_int>::val("\"210\"^^xsd:int")));
    ASSERT_EQ(true, (toCpp<bool, RdfTypeEnum::xsd_boolean>::val("\"true\"^^xsd:boolean")));
    ASSERT_EQ(true, (toCpp<bool, RdfTypeEnum::xsd_boolean>::val("\"1\"^^xsd:boolean")));
    ASSERT_EQ(false, (toCpp<bool, RdfTypeEnum::xsd_boolean>::val("\"0\"^^xsd:boolean")));
    ASSERT_EQ(0.1, (toCpp<double, RdfTypeEnum::xsd_decimal>::val("\"0.1\"^^xsd:decimal")));
    ASSERT_EQ(0.5, (toCpp<float, RdfTypeEnum::xsd_float>::val("\"0.5\"^^xsd:float")));
    ASSERT_EQ(0.5, (toCpp<double, RdfTypeEnum::xsd_double>::val("\"0.5\"^^xsd:double")));
    ASSERT_EQ("1977-Apr-22 06:00:00", boost::posix_time::to_simple_string(toCpp<boost::posix_time::ptime, RdfTypeEnum::xsd_dateTime>::val("\"1977-04-22T06:00:00Z\"^^xsd:dateTime")));

    //Checking trimming
    ASSERT_EQ("date of birth  ", (toCpp<std::string, RdfTypeEnum::xsd_string>::val("\"date of birth  \"^^xsd:string")));
    ASSERT_EQ(true, (toCpp<bool, RdfTypeEnum::xsd_boolean>::val("\"1 \"^^xsd:boolean")));
    ASSERT_EQ(false, (toCpp<bool, RdfTypeEnum::xsd_boolean>::val("\" 0\"^^xsd:boolean")));
    ASSERT_EQ(210, (toCpp<int, RdfTypeEnum::xsd_int>::val("\"\t210\n\"^^xsd:int")));
}

TEST(_04_cvt, cpp_to_xsd) {
    ASSERT_EQ("\"date of birth\"^^xsd:string", (toRdf<std::string, RdfTypeEnum::xsd_string>::val("date of birth")));
    ASSERT_EQ("\"210\"^^xsd:int", (toRdf<int, RdfTypeEnum::xsd_int>::val(210)));
    ASSERT_EQ("\"true\"^^xsd:boolean", (toRdf<bool, RdfTypeEnum::xsd_boolean>::val(true)));
    // That works too
    ASSERT_EQ("\"true\"^^xsd:boolean", (toRdf<int, RdfTypeEnum::xsd_boolean>::val(5)));
    // That works too
    ASSERT_EQ("\"false\"^^xsd:boolean", (toRdf<float, RdfTypeEnum::xsd_boolean>::val(0.0)));

    ASSERT_EQ("\"0.1\"^^xsd:decimal", (toRdf<float, RdfTypeEnum::xsd_decimal>::val(0.1)));
    ASSERT_EQ("\"0.1\"^^xsd:decimal", (toRdf<double, RdfTypeEnum::xsd_decimal>::val(0.1)));
    ASSERT_EQ("\"0.1\"^^xsd:decimal", (toRdf<long double, RdfTypeEnum::xsd_decimal>::val(0.1)));
    ASSERT_EQ("\"0.5\"^^xsd:float", (toRdf<float, RdfTypeEnum::xsd_float>::val(0.5)));
    ASSERT_EQ("\"0.5\"^^xsd:double", (toRdf<double, RdfTypeEnum::xsd_double>::val(0.5)));
    ASSERT_EQ("\"2002-01-20T23:59:59.001000Z\"^^xsd:dateTime", (toRdf<boost::posix_time::ptime, RdfTypeEnum::xsd_dateTime>::val(boost::posix_time::time_from_string("2002-01-20 23:59:59.001"))));
}

// This is cuurently noot supported by boost
TEST(_04_cvt, DISABLED_boostbugs) {
    // This example taken from http://www.w3.org/TR/xmlschema-2/#dateTime fails :-(
    ASSERT_EQ("2002-Oct-10 17:00:00", boost::posix_time::to_simple_string(toCpp<boost::posix_time::ptime, RdfTypeEnum::xsd_dateTime>::val("\"2002-10-10T12:00:00-05:00\"^^xsd:dateTime")));
}