//
// Created by ldimou on 5/4/16.
//

#include <gtest/gtest.h>
#include <boost/filesystem.hpp>

#include "autordf/Factory.h"
#include "autordf/ontology/Ontology.h"
#include "autordf/ontology/Validator.h"

using namespace autordf;
using namespace ontology;

class ValidatorTest : public ::testing::Test {
public:

    autordf::Factory factory;
    Validator* validator;
    void SetUp() {

        Object::setFactory(&factory);
        factory.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/geometry.ttl");
        factory.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/geometry.owl");
        Ontology _ontology(&factory);
        validator = new Validator(_ontology);
    }

    void TearDown( ) {
        delete validator;
    }

};

TEST_F(ValidatorTest, ObjectValidator) {

    std::shared_ptr<std::vector<Validator::Error>> errors = validator->validateObject(Object("http://example.org/geometry/myShape"));

    ASSERT_EQ("http://example.org/geometry#center",  errors->back().property);
    ASSERT_EQ("http://example.org/geometry#radius",  errors->front().property);

    ASSERT_EQ(2, errors->size());
    ASSERT_EQ(2,  errors->front().type);
    ASSERT_EQ(0,  errors->back().count);
}

TEST_F(ValidatorTest, DataType) {

    std::shared_ptr<std::vector<Validator::Error>> errors = validator->validateObject(Object("http://example.org/geometry/point"));

    ASSERT_EQ("http://example.org/geometry#x",  errors->front().property);
    ASSERT_EQ("xsd:double",  errors->front().range);
    ASSERT_EQ(0,  errors->front().type);
}

TEST_F(ValidatorTest, MinCardinality) {

    std::shared_ptr<std::vector<Validator::Error>>  errors = validator->validateObject(Object("http://example.org/geometry/circle"));

    ASSERT_EQ(1, errors->size());
    ASSERT_EQ("http://example.org/geometry#radius", errors->front().property);
    ASSERT_EQ(1, errors->front().val);
    ASSERT_EQ(2,  errors->front().type);
}

TEST_F(ValidatorTest, MaxCardinality) {

    std::shared_ptr<std::vector<Validator::Error>>  errors = validator->validateObject(Object("http://example.org/geometry/rectangle"));

    ASSERT_EQ(1, errors->size());
    ASSERT_EQ("http://example.org/geometry#topLeft", errors->front().property);
    ASSERT_EQ(2,  errors->front().count);
    ASSERT_EQ(3,  errors->front().type);
}

TEST_F(ValidatorTest, Type) {

    std::shared_ptr<std::vector<Validator::Error>>  errors = validator->validateObject(Object("http://example.org/geometry/objectType"));

    ASSERT_EQ(1, errors->size());
    ASSERT_EQ("http://example.org/geometry#center", errors->front().property);
    ASSERT_EQ(1,  errors->front().type);
    ASSERT_EQ("http://example.org/geometry#Point",  errors->front().range);

}

TEST_F(ValidatorTest, ModelValidator) {

    std::shared_ptr<std::vector<Validator::Error>> SError = validator->validateModel(factory);

    EXPECT_EQ(9, SError->size());
    ASSERT_EQ("http://example.org/geometry#textColor", SError->back().property);
    ASSERT_EQ(0,  SError->back().count);
    ASSERT_EQ(2,  SError->back().type);
    ASSERT_EQ(1,  SError->front().type);
    ASSERT_EQ("http://example.org/geometry/text",  SError->back().subject.iri());
    ASSERT_EQ("http://example.org/geometry/objectType",  SError->front().subject.iri());
    ASSERT_EQ("http://example.org/geometry#Point",  SError->front().range);

}