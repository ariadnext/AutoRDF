
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

TEST(_03_Object, Display) {
    Factory f;
    Object::setFactory(&f);

    f.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/foafExample.rdf", "http://xmlns.com/foaf/0.1/");

    std::list<Object> objs = Object::findByType("http://xmlns.com/foaf/0.1/Person");

    Object first = objs.front();

    printRecurse(first, 0);
    printRecurse(first, 1);
}

TEST(_03_Object, Copy) {
    Factory f;
    Object::setFactory(&f);

    f.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/foafExample.rdf", "http://xmlns.com/foaf/0.1/");

    std::list<Object> objs = Object::findByType("http://xmlns.com/foaf/0.1/Person");

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

    std::list<Object> objs = Object::findByType("http://xmlns.com/foaf/0.1/Person");

    Object person = objs.front();

    ASSERT_NO_THROW(person.getObject("http://xmlns.com/foaf/0.1/account"));
    ASSERT_EQ(nullptr, person.getOptionalObject("http://xmlns.com/foaf/0.1/unexisting"));
    ASSERT_EQ(0, person.getObjectList("http://xmlns.com/foaf/0.1/unexisting").size());
    ASSERT_EQ(2, person.getObjectList("http://xmlns.com/foaf/0.1/knows").size());

    Object account = person.getObject("http://xmlns.com/foaf/0.1/account");
    ASSERT_EQ("Jimmy Wales", person.getPropertyValue("http://xmlns.com/foaf/0.1/name"));
    ASSERT_EQ(std::list<PropertyValue>({"Jimmy Wales"}), person.getPropertyValueList("http://xmlns.com/foaf/0.1/name"));
    ASSERT_EQ(std::list<PropertyValue>({"http://xmlns.com/foaf/0.1/OnlineAccount", "http://xmlns.com/foaf/0.1/OnlineChatAccount"}),
              account.getPropertyValueList("http://www.w3.org/1999/02/22-rdf-syntax-ns#type"));

    // Trying to access a value as a resource
    ASSERT_THROW(person.getObject("http://xmlns.com/foaf/0.1/name"), InvalidNodeType);

    // Unexisting object
    ASSERT_THROW(person.getObject("http://xmlns.com/foaf/0.1/unexistingPropery"), PropertyNotFound);

    // Duplicates
    ASSERT_THROW(person.getObject("http://xmlns.com/foaf/0.1/knows"), DuplicateProperty);
    ASSERT_THROW(person.getOptionalObject("http://xmlns.com/foaf/0.1/knows"), DuplicateProperty);
}

TEST(_04_Object, DelayedTypeWriting) {
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

TEST(_04_Object, NoTypeWrittenWhenCloning) {
    Factory f;
    Object::setFactory(&f);

    Object obj("http://myuri/myobject", "http://myuri/type1");

    Object obj2 = obj.clone("http://myuri/myobject2");
    // No statement written that far
    ASSERT_EQ(0, f.find().size());
}

std::map<std::string, std::set<std::string> > rtti = {
        {"http://myuri/childclass1", {"http://myuri/class1"}}
};

class ObjectClass1 : public Object {
public:
    ObjectClass1(const Object& o) : Object(o, "http://myuri/class1", &rtti) {}
    ObjectClass1(const std::string& iri) : Object(iri, "http://myuri/class1", &rtti) {}
};

class ObjectClass2 : public Object {
public:
    ObjectClass2(const Object& o) : Object(o, "http://myuri/class2", &rtti) {}
    ObjectClass2(const std::string& iri) : Object(iri, "http://myuri/class2", &rtti) {}
};

class ChildClass1 : public Object {
public:
    ChildClass1(const Object& o) : Object(o, "http://myuri/childclass1", &rtti) {}
    ChildClass1(const std::string& iri) : Object(iri, "http://myuri/childclass1", &rtti) {}
};

TEST(_04_Object, SubClasses) {
    Factory f;
    Object::setFactory(&f);

    ObjectClass1 objClass1("http://myuri/myobject");

    ObjectClass2 objClass2 = objClass1.as<ObjectClass2>();

    objClass2.setPropertyValue("http://myuri/myprop", "value");
    ASSERT_EQ(2, f.find().size());

    ASSERT_EQ("http://myuri/class2", objClass2.getObject("http://www.w3.org/1999/02/22-rdf-syntax-ns#type").iri());

    ASSERT_THROW(ObjectClass1("http://myuri/myobject"), InvalidClass);

    ChildClass1 childobj("http://myuri/childobject");
    childobj.setPropertyValue("http://myuri/myprop", "value");
    childobj.as<ObjectClass1>();

    ASSERT_THROW(childobj.as<ObjectClass2>(), InvalidClass);
}
