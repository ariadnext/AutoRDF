
#include <gtest/gtest.h>
#include <optional>

#include <boost/filesystem.hpp>

#include "autordf/Factory.h"
#include "autordf/Object.h"
#include "autordf/cvt/Cvt.h"

using namespace autordf;

void printRecurse(const Object& obj, int recurse) {
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

    f.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/foafExample.ttl", "http://xmlns.com/foaf/0.1/");

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

    f.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/foafExample.ttl", "http://xmlns.com/foaf/0.1/");

    std::vector<Object> objs = Object::findByType("http://xmlns.com/foaf/0.1/Person");

    Object first = objs.front();

    ASSERT_EQ(size_t{3}, Object::findByType("http://xmlns.com/foaf/0.1/Person").size());
    Object copy = first.clone("http://personcopy");
    ASSERT_EQ(size_t{4}, Object::findByType("http://xmlns.com/foaf/0.1/Person").size());
    printRecurse(copy, 0);
}

TEST(_03_Object, Accessors) {
    Factory f;
    Object::setFactory(&f);

    f.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/foafExample.ttl", "http://xmlns.com/foaf/0.1/");

    std::vector<Object> objs = Object::findByType("http://xmlns.com/foaf/0.1/Person");

    Object person;
    for ( const auto& p : objs ) {
        if ( p.getPropertyValue("http://xmlns.com/foaf/0.1/name") == "Jimmy Wales" ) {
            person = p;
        }
    }

    ASSERT_NO_THROW(person.getObject("http://xmlns.com/foaf/0.1/account"));
    ASSERT_EQ(std::nullopt, person.getOptionalObject("http://xmlns.com/foaf/0.1/unexisting"));
    ASSERT_TRUE(person.getObjectList("http://xmlns.com/foaf/0.1/unexisting", false).empty());
    ASSERT_EQ(size_t{2}, person.getObjectList("http://xmlns.com/foaf/0.1/knows", false).size());

    Object account = person.getObject("http://xmlns.com/foaf/0.1/account");
    ASSERT_EQ("Jimmy Wales", person.getPropertyValue("http://xmlns.com/foaf/0.1/name"));
    ASSERT_EQ(std::vector<PropertyValue>({"Jimmy Wales"}), person.getPropertyValueList("http://xmlns.com/foaf/0.1/name", false));
    std::vector<Object> ref({Object("http://xmlns.com/foaf/0.1/OnlineChatAccount"), Object("http://xmlns.com/foaf/0.1/OnlineAccount")});
    std::sort(ref.begin(), ref.end());
    auto value = account.getObjectList("http://www.w3.org/1999/02/22-rdf-syntax-ns#type", false);
    std::sort(value.begin(), value.end());
    ASSERT_EQ(ref, value);

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
    ASSERT_TRUE(f.find().empty());
    obj.setPropertyValue("http://myuri/myprop", "value");
    // two statements written
    ASSERT_EQ(size_t{2}, f.find().size());
    ASSERT_EQ("http://myuri/type1", obj.getObject("http://www.w3.org/1999/02/22-rdf-syntax-ns#type").iri());
}

TEST(_03_Object, NoTypeWrittenWhenCloning) {
    Factory f;
    Object::setFactory(&f);

    Object obj("http://myuri/myobject", "http://myuri/type1");

    Object obj2 = obj.clone("http://myuri/myobject2");
    // No statement written that far
    ASSERT_TRUE(f.find().empty());
}

TEST(_03_Object, removeSingleProperty) {
    Factory f;
    Object::setFactory(&f);

    Object obj("http://myuri/myobject", "http://myuri/type1");

    obj.addPropertyValue("http://myuri/prop", "val1", false);
    ASSERT_EQ(size_t{2}, f.find().size());

    obj.removePropertyValue("http://myuri/prop", "val1");
    ASSERT_EQ(size_t{1}, f.find().size());

    long long int toto = 1;
    PropertyValue val;
    val.set<cvt::RdfTypeEnum::xsd_integer>(toto);
    obj.addPropertyValue("http://myuri/prop", val, false);
    ASSERT_EQ(size_t{2}, f.find().size());

    obj.removePropertyValue("http://myuri/prop", val);
    ASSERT_EQ(size_t{1}, f.find().size());

    ASSERT_THROW((obj.removePropertyValue("http://myuri/prop", "val1")), PropertyNotFound);
}

TEST(_03_Object, findByKeyObject) {
    Factory f;
    Object::setFactory(&f);

    f.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/foafExample.ttl", "http://xmlns.com/foaf/0.1/");

    ASSERT_NO_THROW(Object::findByKey("http://xmlns.com/foaf/0.1/mbox", Object("mailto:jwales@bomis.com")));
}

TEST(_03_Object, findByKeyProp) {
    Factory f;
    Object::setFactory(&f);

    f.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/foafExample.ttl", "http://xmlns.com/foaf/0.1/");

    ASSERT_NO_THROW(Object::findByKey("http://xmlns.com/foaf/0.1/nick", "Jimbo"));

    ASSERT_THROW(Object::findByKey("http://xmlns.com/foaf/0.1/nick", "Jimbo2"), ObjectNotFound);
}

TEST(_03_Object, findAllObjects) {
    Factory f;
    Object::setFactory(&f);

    f.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/foafExample.ttl", "http://xmlns.com/foaf/0.1/");

    std::set<Object> objs = Object::findAll();
    ASSERT_EQ(size_t{5}, objs.size());
}

TEST(_03_Object, DataPropertyReification) {
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
    ASSERT_TRUE(obj.getOptionalPropertyValue("http://myprop1").has_value());

    // Test our read back support of reified statements
    ASSERT_EQ("1", obj.getPropertyValue("http://myprop1"));

    // Test our read back support of reified statements
    ASSERT_EQ(std::vector<PropertyValue>({"1"}), obj.getPropertyValueList("http://myprop1", false));

    // Test our read back support of reified statements
    ASSERT_EQ(std::vector< long long int>({1}), (obj.getValueListImpl<cvt::RdfTypeEnum::xsd_integer, long long int>("http://myprop1", false)));

    // Now remove our reified statement
    obj.removePropertyValue("http://myprop1", "1");
}

TEST(_03_Object, DataPropertyReification2) {
    Factory f;
    Object::setFactory(&f);

    Object obj("http://my/object");

    // Check reification does remove our non-reified statement
    obj.setPropertyValue("http://myprop3", "3");
    obj.reifyPropertyValue("http://myprop3", "3");

    EXPECT_EQ(size_t{4}, f.find().size());
}

TEST(_03_Object, DataPropertyUnReification) {
    Factory f;
    Object::setFactory(&f);

    Object obj("http://my/object");
    //Unreifying unexisting should not throw
    ASSERT_NO_THROW(obj.unReifyPropertyValue("http://unexistingprop", "unsexistingvalue"));

    Object reified = obj.reifyPropertyValue("http://myprop1", "1");
    reified.addPropertyValue("http://thirdpartyprop", "thirdpartypropvalue", false);

    f.saveToFile("/tmp/test_UnReification2.ttl");

    ASSERT_THROW(obj.unReifyPropertyValue("http://myprop1", "1"), autordf::CannotUnreify);

    //Now remove statement and test unreification again
    reified.removePropertyValue("http://thirdpartyprop", "thirdpartypropvalue");
    ASSERT_NO_THROW(obj.unReifyPropertyValue("http://myprop1", "1"));

    f.saveToFile("/tmp/test_UnReification3.ttl");
}

TEST(_03_Object, ObjectPropertyReification) {
    Factory f;
    Object::setFactory(&f);

    Object obj("http://my/object");

    Object obj1("http://my/object1");
    Object obj2("http://my/object2");


    obj.setObject("http://myprop2", obj2);
    // At this stage statement is NOT reified, check that
    ASSERT_FALSE(obj.reifiedObject("http://myprop2", obj2));

    obj.reifyObject("http://myprop1", obj1);

    // At this stage statement is reified, check that
    ASSERT_TRUE(obj.reifiedObject("http://myprop1", obj1).get());

    f.saveToFile("/tmp/test_UnReification1.ttl");

    // Test our read back support of reified statements
    ASSERT_TRUE(obj.getOptionalObject("http://myprop1").has_value());

    // Test our read back support of reified statements
    ASSERT_EQ(obj1, obj.getObject("http://myprop1"));

    // Test our read back support of reified statements
    ASSERT_EQ(std::vector<Object>({obj1}), obj.getObjectList("http://myprop1", false));

    // Now remove our reified statement
    obj.removeObject("http://myprop1",  obj1);
}

TEST(_03_Object, ObjectPropertyReification2) {
    Factory f;
    Object::setFactory(&f);

    Object obj("http://my/object");
    Object obj3("http://my/object3");

    // Check reification does remove our non-reified statement
    obj.setObject("http://myprop3", obj3);
    obj.reifyObject("http://myprop3", obj3);

    EXPECT_EQ(size_t{4}, f.find().size());
}

TEST(_03_Object, ObjectPropertyUnReification) {
    Factory f;
    Object::setFactory(&f);

    Object obj("http://my/object");
    //Unreifying unexisting should not throw
    ASSERT_NO_THROW(obj.unReifyObject("http://unexistingprop", Object("http://unexisting")));

    Object obj1("http://my/object1");
    Object reified = obj.reifyObject("http://myprop1", obj1);
    reified.addPropertyValue("http://thirdpartyprop", "thirdpartypropvalue", false);

    f.saveToFile("/tmp/test_UnReification2.ttl");

    ASSERT_THROW(obj.unReifyObject("http://myprop1", obj1), autordf::CannotUnreify);

    //Now remove statement and test unreification again
    reified.removePropertyValue("http://thirdpartyprop", "thirdpartypropvalue");
    ASSERT_NO_THROW(obj.unReifyObject("http://myprop1", obj1));

    f.saveToFile("/tmp/test_UnReification3.ttl");
}

TEST(_03_Object, DataPropertyOrderingVector) {
    Factory f;
    f.addNamespacePrefix("rdf", Object::RDF_NS);
    Object::setFactory(&f);
    Object obj("http://my/object");

    std::vector<PropertyValue> pvv;
    pvv.emplace_back("stringval1");
    pvv.emplace_back("stringval2");
    obj.setPropertyValueList("http://prop1", pvv, true);

    ASSERT_NO_THROW(obj.getPropertyValueList("http://prop1", true));

    std::cout << *f.saveToMemory("turtle") << std::endl;
}

TEST(_03_Object, DataPropertyOrderingAdd) {
    Factory f;
    f.addNamespacePrefix("rdf", Object::RDF_NS);
    Object::setFactory(&f);
    Object obj("http://my/object");

    obj.setPropertyValue("http://prop1", "stringval1");
    obj.addPropertyValue("http://prop1", "stringval2", true);

    std::cout << *f.saveToMemory("turtle") << std::endl;

    ASSERT_THROW(obj.getPropertyValueList("http://prop1", true), autordf::CannotPreserveOrder);
}

TEST(_03_Object, ObjectPropertyOrderingVector) {
    Factory f;
    f.addNamespacePrefix("rdf", Object::RDF_NS);
    Object::setFactory(&f);
    Object obj("http://my/object");

    std::vector<Object> objects;
    objects.emplace_back(Object("http://object1"));
    objects.emplace_back(Object("http://object2"));
    obj.setObjectList("http://prop1", objects, true);

    ASSERT_NO_THROW(obj.getObjectList("http://prop1", true));

    std::cout << *f.saveToMemory("turtle") << std::endl;

    std::vector<Object> read = obj.getObjectList("http://prop1", true);

    std::swap(read.front(), read.back());

    obj.setObjectList("http://prop1", read, true);

    std::cout << *f.saveToMemory("turtle") << std::endl;

    std::vector<Object> read2 = obj.getObjectList("http://prop1", true);

    ASSERT_EQ(size_t{2}, read2.size());

    ASSERT_EQ(objects[0], read2[1]);
    ASSERT_EQ(objects[1], read2[0]);
}

TEST(_03_Object, ObjectPropertyOrderingAdd) {
    Factory f;
    f.addNamespacePrefix("rdf", Object::RDF_NS);
    Object::setFactory(&f);
    Object obj("http://my/object");

    obj.setObject("http://prop1", Object("http://object1"));
    obj.addObject("http://prop1", Object("http://object2"), true);

    std::cout << *f.saveToMemory("turtle") << std::endl;

    ASSERT_THROW(obj.getObjectList("http://prop1", true), autordf::CannotPreserveOrder);
}

TEST(_03_Object, ObjectType) {
    Factory f;
    f.addNamespacePrefix("rdf", Object::RDF_NS);
    Object::setFactory(&f);

    Object obj("http://my/object", "http://myobjecttype");
    obj.writeRdfType();

    ASSERT_TRUE(obj.isA("http://myobjecttype"));

    ASSERT_EQ("http://myobjecttype", obj.getTypes().front());
}

TEST(_03_Object, FindSources) {
    Factory f;
    Object::setFactory(&f);

    f.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/foafExample.ttl", "http://xmlns.com/foaf/0.1/");

    std::vector<Object> objs = Object::findByType("http://xmlns.com/foaf/0.1/Person");

    Object jimmy;
    Object angela;
    for ( auto p : objs ) {
        if ( p.getPropertyValue("http://xmlns.com/foaf/0.1/name") == "Jimmy Wales" ) {
            jimmy = p;
        } else if ( p.getPropertyValue("http://xmlns.com/foaf/0.1/name") == "Angela Beesley" ) {
            angela = p;
        }
    }

    std::set<Object> sources = angela.findSources();
    EXPECT_TRUE(sources.find(angela) == sources.end());
    EXPECT_TRUE(sources.find(jimmy) != sources.end());
}

TEST(_03_Object, FindTargets) {
    Factory f;
    Object::setFactory(&f);

    f.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/foafExample.ttl", "http://xmlns.com/foaf/0.1/");

    std::vector<Object> objs = Object::findByType("http://xmlns.com/foaf/0.1/Person");

    Object person;
    for ( auto p : objs ) {
        if ( p.getPropertyValue("http://xmlns.com/foaf/0.1/name") == "Jimmy Wales" ) {
            person = p;
        }
    }

    std::set<Object> targets = person.findTargets();
    EXPECT_TRUE(targets.find(person) == targets.end());
    Object known = person.getObject("http://xmlns.com/foaf/0.1/knows");
    EXPECT_TRUE(targets.find(known) != targets.end());
}


TEST(_03_Object, SourcesAndTargetsReification) {
    Factory f;
    Object::setFactory(&f);

    Object a("http://objecta");
    Object b("http://objectb");
    a.setObject("http://myprop1", b);
    Object reifiedstatement = a.reifyObject("http://myprop1", b);

    f.saveToFile("/tmp/SourcesAndTargetsReification.ttl");

    ASSERT_EQ(size_t{1}, b.findSources().size());
    EXPECT_EQ(a, *b.findSources().begin());

    ASSERT_EQ(size_t{1}, a.findTargets().size());
    EXPECT_EQ(b, *a.findTargets().begin());
}

TEST(_03_Object, cloneRecursiveStopAtResources) {
    Factory f;
    Object::setFactory(&f);

    f.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/foafExample.ttl", "http://xmlns.com/foaf/0.1/");

    std::vector<Object> objs = Object::findByType("http://xmlns.com/foaf/0.1/Person");

    Object person;
    for ( auto p : objs ) {
        if ( p.getPropertyValue("http://xmlns.com/foaf/0.1/name") == "Jimmy Wales" ) {
            person = p;
        }
    }
    Object clone = person.cloneRecursiveStopAtResources("http://jimmycricket.com/me2");
    Object known = person.getObject("http://xmlns.com/foaf/0.1/knows");
    Object known2 = clone.getObject("http://xmlns.com/foaf/0.1/knows");
    EXPECT_EQ(known, known2);
    objs = Object::findByType("http://xmlns.com/foaf/0.1/Person");
    long count = std::count_if(objs.begin(), objs.end(), [](Object o){ return o.getPropertyValue("http://xmlns.com/foaf/0.1/name") == "Jimmy Wales";});
    EXPECT_EQ(count, 2);
}

TEST(_03_Object, cloneReification) {
    Factory f;
    Object::setFactory(&f);

    Object obj("http://my/object");
    obj.reifyPropertyValue("http://myprop1", "1");
    Object clone = obj.cloneRecursiveStopAtResources("http://my/object2");
    ASSERT_TRUE(clone.reifiedPropertyValue("http://myprop1", "1").get());
}

TEST(_03_Object, cloneReificationViaStandardClone) {
    Factory f;
    Object::setFactory(&f);

    Object obj("http://my/object");
    obj.reifyPropertyValue("http://myprop1", "1");
    Object clone = obj.clone("http://my/object2");
    ASSERT_TRUE(clone.reifiedPropertyValue("http://myprop1", "1").get());
}

TEST(_03_Object, removeObjectRecursive) {
    Factory f;
    Object::setFactory(&f);

    Object obj("http://testns/myobject", "http://testns/type1");

    obj.addPropertyValue("http://property", "val1", false);
    ASSERT_EQ(size_t{2}, f.find().size());

    obj.addPropertyValue("http://property", "val2", false);
    ASSERT_EQ(size_t{3}, f.find().size());

    Object subobject("http://testns/subobj");
    subobject.addPropertyValue("http://property", "val3", false);
    obj.addObject("http://subobject", Object("http://testns/subobject"), false);
    ASSERT_EQ(size_t{5}, f.find().size());

    Object subobject2("http://subobj/bnode");
    Object bnode = obj.reifyObject("http://testns/bnode", subobject2);
    bnode.addPropertyValue("http://property", "val4", false);
    bnode.addObject("http://subobject", Object("http://bnode/subobject"), false);
    obj.addObject("http://testns/obj", bnode, false);
    ASSERT_EQ(size_t{12}, f.find().size());

    obj.remove(true);

    ASSERT_EQ(size_t{1}, f.find().size());  // http://testns/subobj
}

TEST(_03_Object, removeObjectRecursive2) {
    Factory f;
    Object::setFactory(&f);

    Object obj("http://testns/myobject", "http://testns/type1");

    // sub object of type bnode
    Object bnode1("", "http://testns/type2");
    bnode1.addPropertyValue("http://property", "val1", false);
    bnode1.addObject("http://subobject", Object("http://bnode1/subobject"), false);
    obj.addObject("http://subobject", bnode1, false);

    // sub object of type bnode under reified formed
    // --> bnode2 becomes a source of bnode1 as it refers to it by the predicate RDF_SUBJECT
    // bnode1 and bnode2 should be removed nonetheless
    Object subobject2("http://testns/type3");
    Object bnode2 = bnode1.reifyObject("http://testns/reifiedobj", subobject2);
    bnode2.addPropertyValue("http://property", "val2", false);
    bnode2.addObject("http://subobject", Object("http://bnode2/subobject"), false);
    bnode1.addObject("http://subobject", bnode2, false);
    ASSERT_EQ(size_t{12}, f.find().size());

    obj.remove(true);

    std::stringstream debugInfo;
    debugInfo << "Remaining statements:" << std::endl;
    for (auto const& node : f.find()) {
        debugInfo << node << std::endl;
    }

    EXPECT_TRUE(f.find().empty()) << debugInfo.str();
}

TEST(_03_Object, replaceObject) {
    Factory f;
    f.addNamespacePrefix("rdf", Object::RDF_NS);
    Object::setFactory(&f);

    const autordf::Uri propertyIRI("http:///child");
    const autordf::Uri substituteIRI("http://my/child/substitute");
    Object parent("http://my/parent/object");
    Object child(Object("http://my/child/object"));
    Object substitute(substituteIRI);

    parent.setObject(propertyIRI, child);

    ASSERT_NO_THROW(parent.replaceObject(propertyIRI, child, substitute));

    auto obj = parent.getOptionalObject(propertyIRI);
    ASSERT_TRUE(obj != std::nullopt);
    ASSERT_EQ(obj->iri(), substituteIRI);
}

TEST(_03_Object, replaceObjectInList) {
    Factory f;
    f.addNamespacePrefix("rdf", Object::RDF_NS);
    Object::setFactory(&f);

    const autordf::Uri propertyIRI("http://non-ordered/childs");
    Object parent("http://my/parent/object");
    Object bad("http://2object");
    Object good("http://object2");
    std::vector<Object> childs;
    childs.emplace_back(Object("http://object1"));
    childs.emplace_back(bad);
    childs.emplace_back(Object ("http://object3"));
    parent.setObjectList(propertyIRI, childs, false);
    ASSERT_NO_THROW(parent.getObjectList(propertyIRI, false));

    std::vector<Object> read = parent.getObjectList(propertyIRI, false);
    std::vector<autordf::Uri> iris { "http://object1", "http://2object", "http://object3" };
    for (Object o : read) {
        EXPECT_EQ(std::count(iris.begin(), iris.end(), o.iri()), 1);
    }

    parent.replaceObject(propertyIRI, bad, good);

    read = parent.getObjectList(propertyIRI, false);
    iris = { "http://object1", "http://object2", "http://object3" };
    for (Object o : read) {
        EXPECT_EQ(std::count(iris.begin(), iris.end(), o.iri()), 1);
    }
}

TEST(_03_Object, replaceObjectInOrderedList) {
    Factory f;
    f.addNamespacePrefix("rdf", Object::RDF_NS);
    Object::setFactory(&f);

    const autordf::Uri propertyIRI("http://ordered/childs");
    Object parent("http://my/parent/object");
    Object bad("http://fourth");
    Object good("http://second");
    std::vector<Object> childs;
    childs.emplace_back(Object("http://first"));
    childs.emplace_back(bad);
    childs.emplace_back(Object("http://third"));
    parent.setObjectList(propertyIRI, childs, true);
    ASSERT_NO_THROW(parent.getObjectList(propertyIRI, true));

    std::vector<Object> read = parent.getObjectList(propertyIRI, true);
    ASSERT_EQ(childs.size(), read.size());
    EXPECT_EQ(childs[0], read[0]);
    EXPECT_EQ(bad, read[1]);
    EXPECT_EQ(childs[2], read[2]);

    parent.replaceObject(propertyIRI, bad, good);

    ASSERT_NO_THROW(parent.getObjectList(propertyIRI, true));
    read = parent.getObjectList(propertyIRI, true);
    ASSERT_EQ(childs.size(), read.size());
    EXPECT_EQ(childs[0], read[0]);
    EXPECT_EQ(good, read[1]);
    EXPECT_EQ(childs[2], read[2]);
}

TEST(_03_Object, moveObjectInList) {
    Factory f;
    f.addNamespacePrefix("rdf", Object::RDF_NS);
    Object::setFactory(&f);

    const autordf::Uri propertyIRI("http://non-ordered/childs");
    Object parent("http://my/parent/object");
    Object objectToMove("http://objectToMove");
    std::vector<Object> childs;
    childs.emplace_back(Object("http://first"));
    childs.emplace_back(objectToMove);
    childs.emplace_back(Object("http://third"));
    parent.setObjectList(propertyIRI, childs, true);
    //ASSERT_NO_THROW(parent.getPropertyValueList(propertyIRI, false));

    parent.moveObject(propertyIRI, objectToMove, 0);
    std::vector<Object> read = parent.getObjectList(propertyIRI, true);
    std::vector<autordf::Uri> iris { objectToMove.iri(), "http://first", "http://third" };
    for (Object object : read) {
        EXPECT_EQ(std::count(iris.begin(), iris.end(), object), 1);
    }
}

TEST(_03_Object, replacePropertyValue) {
    Factory f;
    f.addNamespacePrefix("rdf", Object::RDF_NS);
    Object::setFactory(&f);

    const autordf::Uri propertyIRI("http:///child");
    autordf::PropertyValue bad("odg");
    autordf::PropertyValue good("dog");
    Object parent("http://my/parent/object");

    parent.setPropertyValue(propertyIRI, bad);

    ASSERT_NO_THROW(parent.replacePropertyValue(propertyIRI, bad, good));

    auto pv = parent.getOptionalPropertyValue(propertyIRI);
    ASSERT_TRUE(pv != std::nullopt);
    ASSERT_EQ(pv, good);
}

TEST(_03_Object, replacePropertyValueInList) {
    Factory f;
    f.addNamespacePrefix("rdf", Object::RDF_NS);
    Object::setFactory(&f);

    const autordf::Uri propertyIRI("http://non-ordered/childs");
    Object parent("http://my/parent/object");
    PropertyValue bad("zozo");
    PropertyValue good("toto");
    std::vector<PropertyValue> childs;
    childs.emplace_back("titi");
    childs.emplace_back(bad);
    childs.emplace_back("tata");
    parent.setPropertyValueList(propertyIRI, childs, false);
    ASSERT_NO_THROW(parent.getPropertyValueList(propertyIRI, false));

    std::vector<PropertyValue> read = parent.getPropertyValueList(propertyIRI, false);
    std::vector<autordf::Uri> iris { "titi", "tata", "zozo" };
    for (PropertyValue pv : read) {
        EXPECT_EQ(std::count(iris.begin(), iris.end(), pv), 1);
    }

    parent.replacePropertyValue(propertyIRI, bad, good);

    read = parent.getPropertyValueList(propertyIRI, false);
    iris = { "titi", "tata", "toto" };
    for (PropertyValue pv : read) {
        EXPECT_EQ(std::count(iris.begin(), iris.end(), pv), 1);
    }
}

TEST(_03_Object, movePropertyValueInList) {
    Factory f;
    f.addNamespacePrefix("rdf", Object::RDF_NS);
    Object::setFactory(&f);

    const autordf::Uri propertyIRI("http://non-ordered/childs");
    Object parent("http://my/parent/object");
    PropertyValue propertyToMove("propertytoMove");
    std::vector<PropertyValue> childs;
    childs.emplace_back("titi");
    childs.emplace_back(propertyToMove);
    childs.emplace_back("tata");
    parent.setPropertyValueList(propertyIRI, childs, true);
    //ASSERT_NO_THROW(parent.getPropertyValueList(propertyIRI, false));

    parent.movePropertyValue(propertyIRI, propertyToMove,2);
    std::vector<PropertyValue> read = parent.getPropertyValueList(propertyIRI, true);
    std::vector<autordf::Uri> iris { "titi", "tata", propertyToMove };
    for (PropertyValue pv : read) {
        EXPECT_EQ(std::count(iris.begin(), iris.end(), pv), 1);
    }

}


TEST(_03_Object, replacePropertyValueInOrderedList) {
    Factory f;
    f.addNamespacePrefix("rdf", Object::RDF_NS);
    Object::setFactory(&f);

    const autordf::Uri propertyIRI("http://ordered/childs");
    Object parent("http://my/parent/object");
    PropertyValue bad("4");
    PropertyValue good("2");
    std::vector<PropertyValue> childs;
    childs.emplace_back(PropertyValue("1"));
    childs.emplace_back(bad);
    childs.emplace_back("3");
    parent.setPropertyValueList(propertyIRI, childs, true);

    ASSERT_NO_THROW(parent.getPropertyValueList(propertyIRI, true));
    std::vector<PropertyValue> read = parent.getPropertyValueList(propertyIRI, true);
    ASSERT_EQ(read[0], "1");
    ASSERT_EQ(read[1], bad);
    ASSERT_EQ(read[2], "3");

    parent.replacePropertyValue(propertyIRI, bad, good);

    ASSERT_NO_THROW(parent.getPropertyValueList(propertyIRI, true));
    read = parent.getPropertyValueList(propertyIRI, true);
    ASSERT_EQ(read[0], "1");
    ASSERT_EQ(read[1], good);
    ASSERT_EQ(read[2], "3");
}

TEST(_03_Object, checkIfPropertyOrdered) {
    Factory f;
    f.addNamespacePrefix("rdf", Object::RDF_NS);
    Object::setFactory(&f);

    Object propertyIRI("http://ordered/childs");
    const Uri ordered("http://github.com/ariadnext/AutoRDF#ordered");

    propertyIRI.setPropertyValue(ordered, "");

    auto stmts = Object::factory()->find();

    EXPECT_TRUE(Object::isPropertyOrdered(propertyIRI.iri()));
}

TEST(_03_Object, recomputeOrderDuringDeletion) {
    Factory f;
    f.addNamespacePrefix("rdf", Object::RDF_NS);
    Object::setFactory(&f);

    Object propertyIRI("http://ordered/childs");
    const Uri ordered("http://github.com/ariadnext/AutoRDF#ordered");
    propertyIRI.setPropertyValue(ordered, "");
    Object parent("http://my/parent/object");
    PropertyValue four("4");
    PropertyValue three("3");
    PropertyValue two("2");
    PropertyValue one("1");
    std::vector<PropertyValue> childs;
    childs.emplace_back(one);
    childs.emplace_back(two);
    childs.emplace_back(three);
    childs.emplace_back(four);
    parent.setPropertyValueList(propertyIRI.iri(), childs, true);
    parent.removePropertyValue(propertyIRI.iri(), three);
    EXPECT_EQ(4, parent.getPropertyValueIndexWithReification(propertyIRI.iri(), four));
    parent.removePropertyValue(propertyIRI.iri(), two, true);
    EXPECT_EQ(2, parent.getPropertyValueIndexWithReification(propertyIRI.iri(), four));
}