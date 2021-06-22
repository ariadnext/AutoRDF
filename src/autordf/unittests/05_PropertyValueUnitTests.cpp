#include <gtest/gtest.h>
#include "autordf/PropertyValue.h"
#include "autordf/cvt/RdfTypeEnum.h"

using namespace autordf;

TEST(_05_PropertyValue, Comparison_xsdString) {
    PropertyValue val1("test", "", "");
    PropertyValue val2(" test", "", "");
    PropertyValue val3("test", "", cvt::rdfTypeIri(cvt::RdfTypeEnum::xsd_string));

    EXPECT_NE(val1, val2); // different lexical form
    EXPECT_EQ(val1, val3); // no datatype and no lang == xsd_string type
}

TEST(_05_PropertyValue, Comparison_xsdInteger) {
    PropertyValue val1("1", "", cvt::rdfTypeIri(cvt::RdfTypeEnum::xsd_integer));
    PropertyValue val2("01", "", cvt::rdfTypeIri(cvt::RdfTypeEnum::xsd_integer));
    PropertyValue val3("1", "", "");
    PropertyValue val4("1", "", cvt::rdfTypeIri(cvt::RdfTypeEnum::xsd_string));

    EXPECT_NE(val1, val2); // different lexical form
    EXPECT_NE(val1, val3); // different datatype (no datatype and no lang == xsd_string type)
    EXPECT_EQ(val3, val4); // same data type
}

TEST(_05_PropertyValue, Comparison_rdfLangString) {
    PropertyValue val1("test", "fr", cvt::rdfTypeIri(cvt::RdfTypeEnum::rdf_langString));
    PropertyValue val2("test", "fr", "");
    PropertyValue val3("test", "en", "");
    PropertyValue val4("test", "", "");

    EXPECT_EQ(val1, val2); // no datatype but lang not empty == rdf_langString type
    EXPECT_NE(val1, val3); // different language tag
    EXPECT_NE(val3, val4); // different data type
}