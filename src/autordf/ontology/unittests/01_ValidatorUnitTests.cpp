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


TEST(_01_Validator, ObjectValidator) {

    Factory factory;
    factory.addNamespacePrefix("geo", "http://example.org/geometry");
    factory.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/geometry.ttl");
    factory.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/geometry.owl");

    Ontology _ontology(&factory);
    Validator validator(_ontology);

    std::shared_ptr<std::vector<Validator::Error>> errors = validator.validateObject(Object("http://example.org/geometry/myShape"));

    ASSERT_EQ("http://example.org/geometry/myShape=> Property 'http://example.org/geometry#center' doesn't have enough values:0. Minimum allowed is 1",
              errors->back().fullMessage());
    ASSERT_EQ("http://example.org/geometry/myShape=> Property 'http://example.org/geometry#radius' doesn't have enough values:0. Minimum allowed is 1",
              errors->front().fullMessage());

    for (auto error: *errors) {
        std::cout << error.fullMessage() << std::endl;
    }
}
TEST(_01_Validator, DataType) {

    Factory factory;
    factory.addNamespacePrefix("geo", "http://example.org/geometry");
    factory.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/geometry.ttl");
    factory.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/geometry.owl");

    Ontology _ontology(&factory);
    Validator validator(_ontology);

    std::shared_ptr<std::vector<Validator::Error>> errors = validator.validateObject(Object("http://example.org/geometry/point"));
    Validator::Error firstError = errors->front();
    for (auto error: *errors) {
        std::cout << error.fullMessage() << std::endl;
    }
    ASSERT_EQ("http://example.org/geometry/point=> Rdf type for the property 'http://example.org/geometry#x' not allowed. Rdf type required: xsd:double",
              firstError.fullMessage());
}

TEST(_01_Validator, MinCardinality) {

    Factory factory;
    factory.addNamespacePrefix("geo", "http://example.org/geometry");
    factory.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/geometry.ttl");
    factory.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/geometry.owl");

    Ontology _ontology(&factory);
    Validator validator(_ontology);

    std::shared_ptr<std::vector<Validator::Error>>  errors = validator.validateObject(Object("http://example.org/geometry/circle"));
    Validator::Error firstError = errors->front();

    ASSERT_EQ(1, errors->size());
    ASSERT_EQ("http://example.org/geometry#radius", firstError.property);
    ASSERT_EQ("http://example.org/geometry/circle=> Property 'http://example.org/geometry#radius' doesn't have enough values:0. Minimum allowed is 1",
              firstError.fullMessage());
}

TEST(_01_Validator, MaxCardinality) {

    Factory factory;
    factory.addNamespacePrefix("geo", "http://example.org/geometry");
    factory.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/geometry.ttl");
    factory.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/geometry.owl");

    Ontology _ontology(&factory);
    Validator validator(_ontology);

    std::shared_ptr<std::vector<Validator::Error>>  errors = validator.validateObject(Object("http://example.org/geometry/rectangle"));
    Validator::Error firstError = errors->front();

    ASSERT_EQ(1, errors->size());
    ASSERT_EQ("http://example.org/geometry#topLeft", firstError.property);
    ASSERT_EQ("http://example.org/geometry/rectangle=> Property 'http://example.org/geometry#topLeft' has too many values:2. Maximum allowed is 1",
              firstError.fullMessage());
}

TEST(_01_Validator, Type) {

    Factory factory;
    factory.addNamespacePrefix("geo", "http://example.org/geometry");
    factory.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/geometry.ttl");
    factory.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/geometry.owl");

    Ontology _ontology(&factory);
    Object::setFactory(&factory);

    Object obj("http://example.org/geometry/type", "http://example.org/geometry#Shape");
    Object objProp;
    objProp.setPropertyValue("http://example.org/geometry#textString", "val");
    obj.setObject("http://example.org/geometry/text", objProp);
    std::cout << obj << std::endl;

    Validator validator(_ontology);

    std::shared_ptr<std::vector<Validator::Error>>  errors = validator.validateObject(obj);
    for (auto error: *errors) {
        std::cout << error.fullMessage() << std::endl;
    }
}

TEST(_01_Validator, ModelValidator) {

    Factory factory;
    factory.addNamespacePrefix("geo", "http://example.org/geometry");
    factory.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/geometry.ttl");
    factory.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/geometry.owl");

    Ontology _ontology(&factory);
    Validator validator(_ontology);

    std::shared_ptr<std::vector<Validator::Error>> SError = validator.validateModel(factory);
    EXPECT_EQ(8, SError->size());
    ASSERT_EQ("http://example.org/geometry/text=> Property 'http://example.org/geometry#textColor' doesn't have enough values:0. Minimum allowed is 1",
              SError->back().fullMessage());
    for (auto error: *SError) {
        std::cout << error.fullMessage() << std::endl;
    }
}