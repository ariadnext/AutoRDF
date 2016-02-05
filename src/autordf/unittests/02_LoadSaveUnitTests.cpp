
#include <gtest/gtest.h>

#include <boost/filesystem.hpp>

#include "autordf/Model.h"
#include "autordf/Factory.h"
#include "autordf/Resource.h"

using namespace autordf;

TEST(_02_LoadSave, OneProperty) {
    Factory f;
    f.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/foafExample.rdf", "http://test");

    Resource r = f.createIRIResource("http://jimmycricket.com/me");
    ASSERT_EQ("Jimmy Criket", r.getProperty("http://xmlns.com/foaf/0.1/name")->value());
    ASSERT_EQ("Jimmy Criket", r.getOptionalProperty("http://xmlns.com/foaf/0.1/name")->value());
}

TEST(_02_LoadSave, PropertyNotThere) {
    Factory f;
    f.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/foafExample.rdf", "http://test");

    Resource r = f.createIRIResource("http://jimmycricket.com/me");
    ASSERT_THROW(r.getProperty("http://notthereuri"), PropertyNotFound);

    ASSERT_EQ(nullptr, r.getOptionalProperty("http://notthereuri"));
}

TEST(_02_LoadSave, AllProperties) {
    Factory f;
    f.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/foafExample.rdf", "http://test");

    Resource r = f.createIRIResource("http://jimmycricket.com/me");
    ASSERT_EQ(2, r.getPropertyValues().size());
}

TEST(_02_LoadSave, loadPerson) {
    Factory f;
    f.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/foafExample.rdf", "http://test");
    Statement req;
    req.object.setLiteral("Jimmy Wales");
    Resource person = f.createResourceFromNode(f.find(req).begin()->subject);
    for (const Property& p : person.getPropertyValues()) {
        std::cout << p << std::endl;
    }
    ASSERT_EQ(14, person.getPropertyValues().size());

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

    Resource test = f.createIRIResource("shortName");
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

    EXPECT_EQ(6, f.find().size());

    f.saveToFile("/tmp/test_deleteProperties.ttl", "http://my/own/");

    drawing.removeSingleProperty(f.createProperty("http://my/own/color")->setValue("blue"));
    ASSERT_EQ(5, f.find().size());

    drawing.removeProperties("http://my/own/color");
    ASSERT_EQ(3, f.find().size());

    drawing.removeProperties("");
    ASSERT_EQ(0, f.find().size());

    ASSERT_THROW(drawing.removeSingleProperty(f.createProperty("http://my/own/color")->setValue("nonexistent")), PropertyNotFound);
}