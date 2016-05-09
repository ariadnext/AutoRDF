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


TEST(_05_Validator, ObjectValidator) {

    std::cout << "Validator Test" << std::endl;
    Factory factory;
    factory.addNamespacePrefix("geo", "http://example.org/geometry");
    factory.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/geometry.ttl");
    factory.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/geometry.owl");

    Ontology _ontology(&factory);
    Object::setFactory(&factory);

    Validator validator(_ontology);

    std::vector<Validator::Error> errors = validator.validateObject(Object("http://example.org/geometry/myShape"));
    for (auto error: errors) {
        std::cout << error.fullMessage() << std::endl;
    }
}
TEST(_05_Validator, DataType) {

    Factory factory;
    factory.addNamespacePrefix("geo", "http://example.org/geometry");
    factory.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/geometry.ttl");
    factory.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/geometry.owl");

    Ontology _ontology(&factory);
    Object::setFactory(&factory);

    Validator validator(_ontology);

    std::vector<Validator::Error> errors = validator.validateObject(Object("http://example.org/geometry/point"));
    for (auto error: errors) {
        std::cout << error.fullMessage() << std::endl;
    }
    ASSERT_EQ("Rdf type for this property http://example.org/geometry#x not allowed. Rdf type required: xsd:double", errors[0].fullMessage());
}

TEST(_05_Validator, MinCardinality) {

    Factory factory;
    factory.addNamespacePrefix("geo", "http://example.org/geometry");
    factory.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/geometry.ttl");
    factory.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/geometry.owl");

    Ontology _ontology(&factory);
    Object::setFactory(&factory);

    Validator validator(_ontology);

    std::vector<Validator::Error> errors = validator.validateObject(Object("http://example.org/geometry/circle"));

    ASSERT_EQ(1, errors.size());
    ASSERT_EQ("http://example.org/geometry#radius", errors[0].property);
    ASSERT_EQ("Property http://example.org/geometry#radius doesn't have enough values: 0. Minimum allowed is 1", errors[0].fullMessage());
}

TEST(_05_Validator, MaxCardinality) {

    Factory factory;
    factory.addNamespacePrefix("geo", "http://example.org/geometry");
    factory.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/geometry.ttl");
    factory.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/geometry.owl");

    Ontology _ontology(&factory);
    Object::setFactory(&factory);

    Validator validator(_ontology);

    std::vector<Validator::Error> errors = validator.validateObject(Object("http://example.org/geometry/rectangle"));

    ASSERT_EQ(1, errors.size());
    ASSERT_EQ("http://example.org/geometry#topLeft", errors[0].property);
    ASSERT_EQ("Property http://example.org/geometry#topLeft has too many values: 2. Maximum allowed is 1", errors[0].fullMessage());
}

TEST(_05_Validator, Type) {

    Factory factory;
    factory.addNamespacePrefix("geo", "http://example.org/geometry");
    factory.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/geometry.ttl");
    factory.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/geometry.owl");

    Ontology _ontology(&factory);
    Object::setFactory(&factory);

    Validator validator(_ontology);

    std::vector<Validator::Error> errors = validator.validateObject(Object("http://example.org/geometry/shape"));
    for (auto error: errors) {
        std::cout << error.fullMessage() << std::endl;
    }
}