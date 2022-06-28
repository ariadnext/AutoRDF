

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
    std::shared_ptr<Validator> validator;
    std::shared_ptr<Ontology> ontology;

    void SetUp() {
        Object::setFactory(&factory);
        factory.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/geometry-ontology.ttl");
        factory.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/geometry.ttl");
        factory.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/schema.ttl");
        ontology = std::make_shared<Ontology>(&factory);
        validator = std::make_shared<Validator>(ontology);
    }

    static void dumpErrors(const std::shared_ptr<std::vector<Validator::Error>>& errors) {
        return; // comment this return to dump errors (usefull when modifying these tests)
        for (auto const& error : *errors) {
            std::cout << error.fullMessage() << std::endl;
        }
    }
};

TEST_F(ValidatorTest, MissingPropertyExactly1) {
    const Object obj("http://example.org/geometry#pointWithoutY");
    const auto errors = validator->validateObject(obj);

    ASSERT_EQ(1, errors->size());
    EXPECT_EQ("'pointWithoutY' property 'y' has 0 distinct values. Minimum allowed is 1", errors->at(0).fullMessage());
    EXPECT_EQ("http://example.org/geometry#y", std::string{errors->at(0).property.iri()});
    EXPECT_EQ(Validator::Error::NOTENOUHVALUES, errors->at(0).type);
}


TEST_F(ValidatorTest, ObjectValidator) {
    const Object obj("http://example.org/geometry#myShape");
    std::shared_ptr<std::vector<Validator::Error>> errors = validator->validateObject(obj);
    dumpErrors(errors);

    ASSERT_EQ(3, errors->size());

    EXPECT_EQ(std::string{"http://example.org/geometry#radius"}, errors->at(0).property.iri());
    EXPECT_EQ(std::string{"'myShape' property 'radius' has 0 distinct values. Minimum allowed is 1"},
              errors->at(0).fullMessage());
    EXPECT_EQ(Validator::Error::NOTENOUHVALUES, errors->at(0).type);

    EXPECT_EQ(std::string{"http://example.org/geometry#center"}, errors->at(1).property.iri());
    EXPECT_EQ(std::string{"'myShape' property 'center' has 0 distinct values. Minimum allowed is 1"},
              errors->at(1).fullMessage());
    EXPECT_EQ(Validator::Error::NOTENOUHVALUES, errors->at(1).type);

    EXPECT_EQ(std::string{"http://example.org/geometry#borderThickness"}, errors->at(2).property.iri());
    EXPECT_EQ(std::string{"'myShape' property 'borderThickness' value 'titi' is not convertible as xsd:integer"},
              errors->at(2).fullMessage());
    EXPECT_EQ(Validator::Error::INVALIDDATATYPE, errors->at(2).type);
}

TEST_F(ValidatorTest, DataType) {
    const Object obj("http://example.org/geometry#point");
    std::shared_ptr<std::vector<Validator::Error>> errors = validator->validateObject(obj);
    dumpErrors(errors);

    ASSERT_EQ(1, errors->size());

    EXPECT_EQ("http://example.org/geometry#x", errors->at(0).property.iri());
    EXPECT_EQ("xsd:double", errors->at(0).range);
    EXPECT_EQ(std::string{"'point' property 'x' value 'toto' is not convertible as xsd:double"},
              errors->at(0).fullMessage());
    EXPECT_EQ(Validator::Error::INVALIDDATATYPE, errors->front().type);
}

TEST_F(ValidatorTest, MinCardinality) {
    const Object obj("http://example.org/geometry#circle");
    std::shared_ptr<std::vector<Validator::Error>> errors = validator->validateObject(obj);
    dumpErrors(errors);

    ASSERT_EQ(2, errors->size());

    EXPECT_EQ("http://example.org/geometry#radius", std::string{errors->at(0).property.iri()});
    EXPECT_EQ(std::string{}, std::string{errors->at(0).range});
    EXPECT_EQ("'circle' property 'radius' has 0 distinct values. Minimum allowed is 1", errors->at(0).fullMessage());
    EXPECT_EQ(Validator::Error::NOTENOUHVALUES, errors->front().type);

    EXPECT_EQ("http://example.org/geometry#borderThickness", std::string{errors->at(1).property.iri()});
    EXPECT_EQ(std::string{}, std::string{errors->at(1).range});
    EXPECT_EQ("'circle' property 'borderThickness' has 0 distinct values. Minimum allowed is 1",
              errors->at(1).fullMessage());
    EXPECT_EQ(Validator::Error::NOTENOUHVALUES, errors->at(1).type);
}

TEST_F(ValidatorTest, MaxCardinality) {
    const Object obj("http://example.org/geometry#rectangle");
    std::shared_ptr<std::vector<Validator::Error>> errors = validator->validateObject(obj);
    dumpErrors(errors);

    ASSERT_EQ(2, errors->size());

    EXPECT_EQ("http://example.org/geometry#topLeft", std::string{errors->at(0).property.iri()});
    EXPECT_EQ(std::string{}, std::string{errors->at(0).range});
    EXPECT_EQ("'rectangle' property 'topLeft' has 2 distinct values. Maximum allowed is 1",
              errors->at(0).fullMessage());
    EXPECT_EQ(Validator::Error::TOOMANYVALUES, errors->at(0).type);

    EXPECT_EQ("http://example.org/geometry#borderThickness", std::string{errors->at(1).property.iri()});
    EXPECT_EQ(std::string{}, std::string{errors->at(1).range});
    EXPECT_EQ("'rectangle' property 'borderThickness' has 0 distinct values. Minimum allowed is 1",
              errors->at(1).fullMessage());
    EXPECT_EQ(Validator::Error::NOTENOUHVALUES, errors->at(1).type);
}

TEST_F(ValidatorTest, Type) {
    const Object obj("http://example.org/geometry#objectType");
    std::shared_ptr<std::vector<Validator::Error>> errors = validator->validateObject(obj);
    dumpErrors(errors);

    ASSERT_EQ(3, errors->size());

    EXPECT_EQ("http://example.org/geometry#center", std::string{errors->at(0).property.iri()});
    EXPECT_EQ("http://example.org/geometry#Point", std::string{errors->at(0).range});
    EXPECT_EQ(
            "'http://example.org/point/center' property 'center' has no type, while RDF expected type is http://example.org/geometry#Point",
            errors->at(0).fullMessage());
    EXPECT_EQ(Validator::Error::INVALIDTYPE, errors->at(0).type);

    EXPECT_EQ("http://example.org/geometry#borderThickness", std::string{errors->at(1).property.iri()});
    EXPECT_EQ(std::string{}, std::string{errors->at(1).range});
    EXPECT_EQ(
            "'objectType' property 'borderThickness' has 0 distinct values. Minimum allowed is 1",
            errors->at(1).fullMessage());
    EXPECT_EQ(Validator::Error::NOTENOUHVALUES, errors->at(1).type);

    EXPECT_EQ("http://example.org/geometry#center", std::string{errors->at(2).property.iri()});
    EXPECT_EQ("http://example.org/geometry#Point", std::string{errors->at(2).range});
    EXPECT_EQ(
            "'http://example.org/point/center' property 'center' has no type, while RDF expected type is http://example.org/geometry#Point",
            errors->at(2).fullMessage());
    EXPECT_EQ(Validator::Error::INVALIDTYPE, errors->at(2).type);

}

TEST_F(ValidatorTest, UniqueKeyValidator) {
    const Object obj("http://example.org/geometry#rectangle1");
    std::shared_ptr<std::vector<Validator::Error>> errors = validator->validateObject(obj);
    ASSERT_EQ(0, errors->size());
}

TEST_F(ValidatorTest, UniqueKeyValidatorFail) {
    const Object obj("http://example.org/geometry#schema2");

    std::shared_ptr<std::vector<Validator::Error>> errors = validator->validateObject(obj);
    EXPECT_EQ(0, errors->size());

    Validator::ValidationOption option = {true};
    errors = validator->validateObject(obj, option);
    ASSERT_EQ(1, errors->size());
    EXPECT_EQ(Validator::Error::DUPLICATEDVALUESKEY, errors->at(0).type);
    EXPECT_EQ("\'Schema\' class key \'schemaUniqueName\' has the duplicated value \'sharedSchemaKey\'", errors->at(0).fullMessage());

    // The error is indeed catched at runtime
    EXPECT_THROW(
            Object::findByKey("http://example.org/geometry#schemaUniqueName", "sharedSchemaKey"),
            autordf::DuplicateObject
    );

}

TEST_F(ValidatorTest, UniqueKeyValidatorFailForOtherInstance) {
    const Object obj("http://example.org/geometry#schema1");

    Validator::ValidationOption option = {true};
    std::shared_ptr<std::vector<Validator::Error>> errors = validator->validateObject(obj, option);
    ASSERT_EQ(1, errors->size());
    EXPECT_EQ(Validator::Error::DUPLICATEDVALUESKEY, errors->at(0).type);
    EXPECT_EQ("\'Schema\' class key \'schemaUniqueName\' has the duplicated value \'sharedSchemaKey\'", errors->at(0).fullMessage());
}

TEST_F(ValidatorTest, ModelValidator) {
    Validator::ValidationOption option = {true};
    std::shared_ptr<std::vector<Validator::Error>> errors = validator->validateModel(factory, option);
    dumpErrors(errors);
    EXPECT_EQ(19, errors->size());

    errors = validator->validateModel(factory);
    dumpErrors(errors);
    EXPECT_EQ(16, errors->size());
}


