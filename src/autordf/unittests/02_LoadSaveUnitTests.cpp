
#include <gtest/gtest.h>

#include <boost/filesystem.hpp>

#include "autordf/Model.h"
#include "autordf/Factory.h"
#include "autordf/Resource.h"

using namespace autordf;

TEST(_02_LoadSave, OneProperty) {
    Factory f;
    f.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/foafExample.ttl", "http://test");

    Resource r = f.createIRIResource("http://jimmycricket.com/me");
    ASSERT_EQ("Jimmy Criket", r.getProperty("http://xmlns.com/foaf/0.1/name")->value());
    ASSERT_EQ("Jimmy Criket", r.getOptionalProperty("http://xmlns.com/foaf/0.1/name")->value());
    std::shared_ptr<Property> p = f.createProperty("http://xmlns.com/foaf/0.1/name");
    p->setValue("Jimmy Criket");
    ASSERT_TRUE(r.hasProperty(*p));
}

TEST(_02_LoadSave, PropertyNotThere) {
    Factory f;
    f.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/foafExample.ttl", "http://test");

    Resource r = f.createIRIResource("http://jimmycricket.com/me");
    ASSERT_THROW(r.getProperty("http://notthereuri"), PropertyNotFound);

    ASSERT_EQ(std::nullopt, r.getOptionalProperty("http://notthereuri"));

    std::shared_ptr<Property> p = f.createProperty("http://notthereuri");
    p->setValue("1");
    ASSERT_FALSE(r.hasProperty(*p));
}

TEST(_02_LoadSave, AllProperties) {
    Factory f;
    f.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/foafExample.ttl", "http://test");

    Resource r = f.createIRIResource("http://jimmycricket.com/me");
    ASSERT_EQ(size_t{2}, r.getPropertyValues()->size());
}

TEST(_02_LoadSave, loadPerson) {
    Factory f;
    f.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/foafExample.ttl", "http://test");
    Statement req;
    req.object.setLiteral("Jimmy Wales");
    Resource person = f.createResourceFromNode(f.find(req).begin()->subject);
    auto values = person.getPropertyValues();
    for (const Property& p : *values) {
        std::cout << p << std::endl;
    }
    ASSERT_EQ(size_t{14}, person.getPropertyValues()->size());

    ASSERT_EQ("jwales", person
            .getProperty("http://xmlns.com/foaf/0.1/account")->asResource()
            .getProperty("http://xmlns.com/foaf/0.1/accountName")->value());
}

TEST(_02_LoadSave, saveResource) {
    Factory f;

    Resource person = f.createIRIResource("http://my/own/resource");
    Resource place = f.createBlankNodeResource();
    std::shared_ptr<Property> myPlaceProp = f.createProperty("http://my/own/placeprop");
    myPlaceProp->setValue(place);
    person.addProperty(*myPlaceProp);

    std::shared_ptr<Property> placeName = f.createProperty("http://my/own/placename");
    placeName->setValue("Place des pizzas");
    place.addProperty(*placeName);

    Resource test = f.createIRIResource("http://shortName");
    test.addProperty(*placeName);

    f.saveToFile("/tmp/test_saveResource.ttl", "http://my/own/");
}

TEST(_02_LoadSave, deleteProperties) {
    Factory f;

    Resource drawing = f.createIRIResource("http://my/own/drawing");

    drawing.addProperty(f.createProperty("http://my/own/color")->setValue("red"));
    drawing.addProperty(f.createProperty("http://my/own/color")->setValue("green"));
    drawing.addProperty(f.createProperty("http://my/own/color")->setValue("blue"));
    drawing.addProperty(f.createProperty("http://my/own/shape")->setValue("circle"));
    drawing.addProperty(f.createProperty("http://my/own/shape")->setValue("rectangle"));
    drawing.addProperty(f.createProperty("http://my/own/backround")->setValue("dots"));

    EXPECT_EQ(size_t{6}, f.find().size());

    f.saveToFile("/tmp/test_deleteProperties.ttl", "http://my/own/");

    drawing.removeSingleProperty(f.createProperty("http://my/own/color")->setValue("blue"));
    ASSERT_EQ(size_t{5}, f.find().size());

    drawing.removeProperties("http://my/own/color");
    f.saveToFile("/tmp/test_deleteProperties2.ttl", "http://my/own/");
    ASSERT_EQ(size_t{3}, f.find().size());

    drawing.removeProperties("");
    ASSERT_TRUE(f.find().empty());

    ASSERT_THROW(drawing.removeSingleProperty(f.createProperty("http://my/own/color")->setValue("nonexistent")), PropertyNotFound);
}