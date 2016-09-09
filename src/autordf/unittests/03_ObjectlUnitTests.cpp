
#include <gtest/gtest.h>

#include <boost/filesystem.hpp>

#include "autordf/Factory.h"
#include "autordf/Object.h"

using namespace autordf;

void printRecurse(Object obj, int recurse) {
    std::cout << "Printing with recurse " << recurse << std::endl;
    std::cout << "----------------------------------------------" << std::endl;
    std::cout << "{" << std::endl;
    obj.printStream(std::cout, recurse, 1) << std::endl;
    std::cout << "}" << std::endl;
    std::cout << "----------------------------------------------" << std::endl;
}

TEST(_03_Object, Comparison) {
    Factory f;
    Object::setFactory(&f);

    Object obj1;
    Object obj2;
    ASSERT_TRUE(obj1 != obj2);

    obj2 = obj1;
    ASSERT_TRUE(obj1 == obj2);
}

TEST(_03_Object, Display) {
    Factory f;
    Object::setFactory(&f);

    f.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/foafExample.rdf", "http://xmlns.com/foaf/0.1/");

    std::vector<Object> objs = Object::findByType("http://xmlns.com/foaf/0.1/Person");

    Object first = objs.front();

    printRecurse(first, 0);
    printRecurse(first, 1);
}

TEST(_03_Object, QName) {
    Factory f;
    Object::setFactory(&f);

    f.addNamespacePrefix("my", "http://my/");

    Object o("http://my/thing");

    ASSERT_EQ("my:thing", o.QName());
}

TEST(_03_Object, Copy) {
    Factory f;
    Object::setFactory(&f);

    f.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/foafExample.rdf", "http://xmlns.com/foaf/0.1/");

    std::vector<Object> objs = Object::findByType("http://xmlns.com/foaf/0.1/Person");

    Object first = objs.front();

    ASSERT_EQ(3, Object::findByType("http://xmlns.com/foaf/0.1/Person").size());
    Object copy = first.clone("http://personcopy");
    ASSERT_EQ(4, Object::findByType("http://xmlns.com/foaf/0.1/Person").size());
    printRecurse(copy, 0);
}

TEST(_03_Object, Accessors) {
    Factory f;
    Object::setFactory(&f);

    f.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/foafExample.rdf", "http://xmlns.com/foaf/0.1/");

    std::vector<Object> objs = Object::findByType("http://xmlns.com/foaf/0.1/Person");

    Object person = objs[2];

    ASSERT_NO_THROW(person.getObject("http://xmlns.com/foaf/0.1/account"));
    ASSERT_EQ(nullptr, person.getOptionalObject("http://xmlns.com/foaf/0.1/unexisting"));
    ASSERT_EQ(0, person.getObjectList("http://xmlns.com/foaf/0.1/unexisting").size());
    ASSERT_EQ(2, person.getObjectList("http://xmlns.com/foaf/0.1/knows").size());

    Object account = person.getObject("http://xmlns.com/foaf/0.1/account");
    ASSERT_EQ("Jimmy Wales", person.getPropertyValue("http://xmlns.com/foaf/0.1/name"));
    ASSERT_EQ(std::vector<PropertyValue>({"Jimmy Wales"}), person.getPropertyValueList("http://xmlns.com/foaf/0.1/name"));
    ASSERT_EQ(std::vector<Object>({Object("http://xmlns.com/foaf/0.1/OnlineChatAccount"), Object("http://xmlns.com/foaf/0.1/OnlineAccount")}),
              account.getObjectList("http://www.w3.org/1999/02/22-rdf-syntax-ns#type"));

    // Trying to access a value as a resource
    ASSERT_THROW(person.getObject("http://xmlns.com/foaf/0.1/name"), InvalidNodeType);

    // Unexisting object
    ASSERT_THROW(person.getObject("http://xmlns.com/foaf/0.1/unexistingPropery"), PropertyNotFound);
}

TEST(_03_Object, DelayedTypeWriting) {
    Factory f;
    Object::setFactory(&f);

    Object obj("http://myuri/myobject", "http://myuri/type1");

    // No statement written that far
    ASSERT_EQ(0, f.find().size());
    obj.setPropertyValue("http://myuri/myprop", "value");
    // two statements written
    ASSERT_EQ(2, f.find().size());
    ASSERT_EQ("http://myuri/type1", obj.getObject("http://www.w3.org/1999/02/22-rdf-syntax-ns#type").iri());
}

TEST(_03_Object, NoTypeWrittenWhenCloning) {
    Factory f;
    Object::setFactory(&f);

    Object obj("http://myuri/myobject", "http://myuri/type1");

    Object obj2 = obj.clone("http://myuri/myobject2");
    // No statement written that far
    ASSERT_EQ(0, f.find().size());
}

TEST(_03_Object, removeSingleProperty) {
    Factory f;
    Object::setFactory(&f);

    Object obj("http://myuri/myobject", "http://myuri/type1");

    obj.addPropertyValue("http://myuri/prop", "val1");
    ASSERT_EQ(2, f.find().size());

    obj.removePropertyValue("http://myuri/prop", "val1");
    ASSERT_EQ(1, f.find().size());

    long long int toto = 1;
    PropertyValue val;
    val.set<cvt::RdfTypeEnum::xsd_integer>(toto);
    obj.addPropertyValue("http://myuri/prop", val);
    ASSERT_EQ(2, f.find().size());

    obj.removePropertyValue("http://myuri/prop", val);
    ASSERT_EQ(1, f.find().size());

    ASSERT_THROW((obj.removePropertyValue("http://myuri/prop", "val1")), PropertyNotFound);
}

TEST(_03_Object, findByKeyProp) {
    Factory f;
    Object::setFactory(&f);

    f.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/foafExample.rdf", "http://xmlns.com/foaf/0.1/");

    ASSERT_NO_THROW(Object::findByKey("http://xmlns.com/foaf/0.1/mbox", Object("mailto:jwales@bomis.com")));
}

TEST(_03_Object, findByKeyObject) {
    Factory f;
    Object::setFactory(&f);

    f.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/foafExample.rdf", "http://xmlns.com/foaf/0.1/");

    ASSERT_NO_THROW(Object::findByKey("http://xmlns.com/foaf/0.1/nick", "Jimbo"));

    ASSERT_THROW(Object::findByKey("http://xmlns.com/foaf/0.1/nick", "Jimbo2"), ObjectNotFound);
}

TEST(_03_Object, findAllObjects) {
    Factory f;
    Object::setFactory(&f);

    f.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/foafExample.rdf", "http://xmlns.com/foaf/0.1/");

    std::set<Object> objs = Object::findAll();
    ASSERT_EQ(5, objs.size());
}

TEST(_03_Object, Reification) {
    Factory f;
    Object::setFactory(&f);

    Object obj("http://my/object");

    obj.setPropertyValue("http://myprop2", "2");
    // At this stage statement is NOT reified, check that
    ASSERT_FALSE(obj.reifiedPropertyValue("http://myprop2", "2"));

    obj.reifyPropertyValue("http://myprop1", "1");

    // At this stage statement is reified, check that
    ASSERT_TRUE(obj.reifiedPropertyValue("http://myprop1", "1").get());

    f.saveToFile("/tmp/test_UnReification1.ttl");

    // Test our read back support of reified statements
    ASSERT_TRUE(obj.getOptionalPropertyValue("http://myprop1").get());

    // Test our read back support of reified statements
    ASSERT_EQ("1", obj.getPropertyValue("http://myprop1"));

    // Test our read back support of reified statements
    ASSERT_EQ(std::vector<PropertyValue>({"1"}), obj.getPropertyValueList("http://myprop1"));

    // Now remove our reified statement
    obj.removePropertyValue("http://myprop1", "1");
}

TEST(_03_Object, Reification2) {
    Factory f;
    Object::setFactory(&f);

    Object obj("http://my/object");

    // Check reification does remove our non-reified statement
    obj.setPropertyValue("http://myprop3", "3");
    obj.reifyPropertyValue("http://myprop3", "3");

    EXPECT_EQ(4, f.find().size());
}

TEST(_03_Object, UnReification) {
    Factory f;
    Object::setFactory(&f);

    Object obj("http://my/object");
    //Unreifying unexisting should not throw
    ASSERT_NO_THROW(obj.unReifyPropertyValue("http://unexistingprop", "unsexistingvalue"));

    Object reified = obj.reifyPropertyValue("http://myprop1", "1");
    reified.addPropertyValue("http://thirdpartyprop", "thirdpartypropvalue");

    f.saveToFile("/tmp/test_UnReification2.ttl");

    ASSERT_THROW(obj.unReifyPropertyValue("http://myprop1", "1"), autordf::CannotUnreify);

    //Now remove statement and test unreification again
    reified.removePropertyValue("http://thirdpartyprop", "thirdpartypropvalue");
    ASSERT_NO_THROW(obj.unReifyPropertyValue("http://myprop1", "1"));

    f.saveToFile("/tmp/test_UnReification3.ttl");
}
