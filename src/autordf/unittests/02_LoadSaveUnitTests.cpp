
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
    p->setValue(PropertyValue("Jimmy Criket", datatype::DATATYPE_STRING));
    ASSERT_TRUE(r.hasProperty(*p));
}

TEST(_02_LoadSave, PropertyNotThere) {
    Factory f;
    f.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/foafExample.ttl", "http://test");

    Resource r = f.createIRIResource("http://jimmycricket.com/me");
    ASSERT_THROW(r.getProperty("http://notthereuri"), PropertyNotFound);

    ASSERT_EQ(nullptr, r.getOptionalProperty("http://notthereuri"));

    std::shared_ptr<Property> p = f.createProperty("http://notthereuri");
    p->setValue(PropertyValue(1));
    ASSERT_FALSE(r.hasProperty(*p));
}

TEST(_02_LoadSave, AllProperties) {
    Factory f;
    f.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/foafExample.ttl", "http://test");

    Resource r = f.createIRIResource("http://jimmycricket.com/me");
    ASSERT_EQ(2, r.getPropertyValues()->size());
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
    ASSERT_EQ(14, person.getPropertyValues()->size());

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
    placeName->setValue(PropertyValue("Place des pizzas","http://www.ariadnext.com/datatype#test"));
    place.addProperty(*placeName);

    Resource test = f.createIRIResource("http://shortName");
    test.addProperty(*placeName);

    f.saveToFile("/tmp/test_saveResource.ttl", "http://my/own/");
}

TEST(_02_LoadSave, deleteProperties) {
    Factory f;

    Resource drawing = f.createIRIResource("http://my/own/drawing");

    drawing.addProperty(f.createProperty("http://my/own/color")->setValue(PropertyValue("red","http://www.ariadnext.com/datatype#color")));
    drawing.addProperty(f.createProperty("http://my/own/color")->setValue(PropertyValue("green","http://www.ariadnext.com/datatype#color")));
    drawing.addProperty(f.createProperty("http://my/own/color")->setValue(PropertyValue("blue","http://www.ariadnext.com/datatype#color")));
    drawing.addProperty(f.createProperty("http://my/own/shape")->setValue(PropertyValue("circle","http://www.ariadnext.com/datatype#shape")));
    drawing.addProperty(f.createProperty("http://my/own/shape")->setValue(PropertyValue("rectangle","http://www.ariadnext.com/datatype#shape")));
    drawing.addProperty(f.createProperty("http://my/own/backround")->setValue(PropertyValue("dots","http://www.ariadnext.com/datatype#backround")));

    EXPECT_EQ(6, f.find().size());

    f.saveToFile("/tmp/test_deleteProperties.ttl", "http://my/own/");

    drawing.removeSingleProperty(f.createProperty("http://my/own/color")->setValue(PropertyValue("blue","http://www.ariadnext.com/datatype#color")));
    ASSERT_EQ(5, f.find().size());

    drawing.removeProperties("http://my/own/color");
    f.saveToFile("/tmp/test_deleteProperties2.ttl", "http://my/own/");
    ASSERT_EQ(3, f.find().size());

    drawing.removeProperties("");
    ASSERT_EQ(0, f.find().size());

    ASSERT_THROW(drawing.removeSingleProperty(f.createProperty("http://my/own/color")->setValue(PropertyValue("nonexistent","http://www.ariadnext.com/datatype#color"))), PropertyNotFound);
}


TEST(_02_LoadSave, TestPropertyOfTypeExist) {
    Factory f;

    Resource drawing = f.createIRIResource("http://my/own/drawing");

    Property p1 = f.createProperty("http://my/own/color")->setValue(PropertyValue("red","http://www.ariadnext.com/datatype#color"));
    Property p2 = f.createProperty("http://my/own/color")->setValue(PropertyValue("red", datatype::DATATYPE_STRING));
    drawing.addProperty(p1);
    ASSERT_TRUE(drawing.hasProperty(p1));
    ASSERT_FALSE(drawing.hasProperty(p2));

}

TEST(_02_LoadSave, compareProperties) {

    PropertyValue red1("red","http://www.ariadnext.com/datatype#color");
    PropertyValue red2("red","http://www.ariadnext.com/datatype#color");
    PropertyValue red3("red","");

    ASSERT_TRUE(red1 == red2);
    ASSERT_TRUE(red1 != red3);

    PropertyValue test1("test", autordf::datatype::DATATYPE_LANGSTRING, "en");
    PropertyValue test2("test", autordf::datatype::DATATYPE_LANGSTRING, "en");
    PropertyValue test3("test", autordf::datatype::DATATYPE_LANGSTRING, "fr");
    PropertyValue test4("test", autordf::datatype::DATATYPE_STRING);

    ASSERT_TRUE(test1 == test2);
    ASSERT_TRUE(test1 != test3);
    ASSERT_TRUE(test1 != test4);
    ASSERT_TRUE(test3 != test4);

    PropertyValue a("1", datatype::DATATYPE_INTEGER);
    PropertyValue b(1);
    ASSERT_TRUE(a == b);
}
