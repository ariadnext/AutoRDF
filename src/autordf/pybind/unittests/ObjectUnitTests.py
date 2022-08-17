import unittest
import autordf_py
import os

class TestObject(unittest.TestCase):
    def test_comparison(self):
        fact = autordf_py.Factory()
        autordf_py.Object.setFactory(fact)
        obj1 = autordf_py.Object()
        obj2 = autordf_py.Object()
        self.assertNotEqual(obj1, obj2)
        obj2 = obj1
        self.assertEqual(obj1, obj2)

    def test_QName(self):
        fact = autordf_py.Factory()
        autordf_py.Object.setFactory(fact)
        fact.addNamespacePrefix("my", "http://my/")
        obj = autordf_py.Object("http://my/thing")
        self.assertEqual(obj.QName(), "my:thing")

    def test_copy(self):
        fact = autordf_py.Factory()
        autordf_py.Object.setFactory(fact)
        fact.loadFromFile(os.path.dirname(os.path.abspath(__file__)) + "/foafExample.ttl", "http://xmlns.com/foaf/0.1/")
        objs = autordf_py.Object.findByType("http://xmlns.com/foaf/0.1/Person")
        self.assertEqual(len(objs), 3)
        first = objs[0]
        copy = first.clone("http://personcopy")
        objs = autordf_py.Object.findByType("http://xmlns.com/foaf/0.1/Person")
        self.assertEqual(len(objs), 4)

    def test_accessors(self):
        fact = autordf_py.Factory()
        autordf_py.Object.setFactory(fact)
        fact.loadFromFile(os.path.dirname(os.path.abspath(__file__)) + "/foafExample.ttl", "http://xmlns.com/foaf/0.1/")
        objs = autordf_py.Object.findByType("http://xmlns.com/foaf/0.1/Person")
        for obj in objs:
            if obj.getPropertyValue("http://xmlns.com/foaf/0.1/name") == "Jimmy Wales":
                person = obj
                break
        person.getObject("http://xmlns.com/foaf/0.1/account")
        self.assertEqual(None, person.getOptionalObject("http://xmlns.com/foaf/0.1/unexisting"))
        self.assertFalse(person.getObjectList("http://xmlns.com/foaf/0.1/unexisting", False))
        self.assertEqual(2, len(person.getObjectList("http://xmlns.com/foaf/0.1/knows", False)))
        account = person.getObject("http://xmlns.com/foaf/0.1/account")
        self.assertEqual("Jimmy Wales", person.getPropertyValue("http://xmlns.com/foaf/0.1/name"))
        self.assertEqual(["Jimmy Wales"], person.getPropertyValueList("http://xmlns.com/foaf/0.1/name", False))
        listRef = [autordf_py.Object("http://xmlns.com/foaf/0.1/OnlineChatAccount"), autordf_py.Object("http://xmlns.com/foaf/0.1/OnlineAccount")]
        listTest = account.getObjectList("http://www.w3.org/1999/02/22-rdf-syntax-ns#type", False)
        self.assertEqual(sorted(listRef), sorted(listTest))
        self.assertRaises(autordf_py.InvalidNodeType, person.getObject, "http://xmlns.com/foaf/0.1/name")
        self.assertRaises(autordf_py.PropertyNotFound, person.getObject, "http://xmlns.com/foaf/0.1/unexistingPropery")

    def test_delayed_typewriting(self):
        fact = autordf_py.Factory()
        autordf_py.Object.setFactory(fact)
        obj = autordf_py.Object("http://myuri/myobject", "http://myuri/type1")
        self.assertFalse(fact.findSize())
        obj.setPropertyValue("http://myuri/myprop", "value")
        self.assertEqual(2, fact.findSize())
        self.assertEqual("http://myuri/type1", obj.getObject("http://www.w3.org/1999/02/22-rdf-syntax-ns#type").iri())

    def test_no_type_written_when_cloning(self):
        fact = autordf_py.Factory()
        autordf_py.Object.setFactory(fact)
        obj = autordf_py.Object("http://myuri/myobject", "http://myuri/type1")
        obj2 = obj.clone("http://myuri/myobject2")
        self.assertFalse(fact.findSize())

    def test_remove_single_property(self):
        fact = autordf_py.Factory()
        autordf_py.Object.setFactory(fact)
        obj = autordf_py.Object("http://myuri/myobject", "http://myuri/type1")

        obj.addPropertyValue("http://myuri/prop", "val1", False)
        self.assertEqual(2, fact.findSize())
        obj.removePropertyValue("http://myuri/prop","val1")
        self.assertEqual(1, fact.findSize())

        val = autordf_py.PropertyValue()
        val.set(1)
        obj.addPropertyValue("http://myuri/prop", val, False)
        self.assertEqual(2, fact.findSize())
        obj.removePropertyValue("http://myuri/prop", val)
        self.assertEqual(1, fact.findSize())
        self.assertRaises(autordf_py.PropertyNotFound, obj.removePropertyValue, "http://myuri/prop", val)

    def test_find_by_key_object(self):
        fact = autordf_py.Factory()
        autordf_py.Object.setFactory(fact)
        fact.loadFromFile(os.path.dirname(os.path.abspath(__file__)) + "/foafExample.ttl", "http://xmlns.com/foaf/0.1/")
        obj = autordf_py.Object("mailto:jwales@bomis.com")
        autordf_py.Object.findByKey("http://xmlns.com/foaf/0.1/mbox", obj)

    def test_find_by_key_prop(self):
        fact = autordf_py.Factory()
        autordf_py.Object.setFactory(fact)
        fact.loadFromFile(os.path.dirname(os.path.abspath(__file__)) + "/foafExample.ttl", "http://xmlns.com/foaf/0.1/")
        autordf_py.Object.findByKey("http://xmlns.com/foaf/0.1/nick", "Jimbo")
        self.assertRaises(autordf_py.ObjectNotFound, autordf_py.Object.findByKey, "http://xmlns.com/foaf/0.1/nick", "Jimbo2")

    def test_find_all_objects(self):
        fact = autordf_py.Factory()
        autordf_py.Object.setFactory(fact)
        fact.loadFromFile(os.path.dirname(os.path.abspath(__file__)) + "/foafExample.ttl", "http://xmlns.com/foaf/0.1/")
        res = autordf_py.Object.findAll()
        self.assertEqual(5, len(res))

    def test_data_property_reification(self):
        fact = autordf_py.Factory()
        autordf_py.Object.setFactory(fact)
        obj = autordf_py.Object("http://my/object")
        obj.setPropertyValue("http://myprop2", "2")
        self.assertFalse(obj.reifiedPropertyValue("http://myprop2", "2"))
        obj.reifyPropertyValue("http://myprop1", "1")
        self.assertTrue(obj.reifiedPropertyValue("http://myprop1", "1"))

        self.assertTrue(obj.getOptionalPropertyValue("http://myprop1"))
        self.assertEqual("1", obj.getPropertyValue("http://myprop1"))
        self.assertEqual(["1"], obj.getPropertyValueList("http://myprop1", False))
        obj.removePropertyValue("http://myprop1", "1")

    def test_data_property_reification2(self):
        fact = autordf_py.Factory()
        autordf_py.Object.setFactory(fact)
        obj = autordf_py.Object("http://my/object")
        obj.setPropertyValue("http://myprop3", "3")
        obj.reifyPropertyValue("http://myprop3", "3")
        self.assertEqual(4, fact.findSize())

    def test_data_property_unreification(self):
        fact = autordf_py.Factory()
        autordf_py.Object.setFactory(fact)
        obj = autordf_py.Object("http://my/object")
        obj.unReifyPropertyValue("http://unexistingprop", "unsexistingvalue")
        reified = obj.reifyPropertyValue("http://myprop1", "1")
        reified.addPropertyValue("http://thirdpartyprop", "thirdpartypropvalue", False)
        self.assertRaises(autordf_py.CannotUnreify, obj.unReifyPropertyValue, "http://myprop1", "1")
        reified.removePropertyValue("http://thirdpartyprop", "thirdpartypropvalue")
        obj.unReifyPropertyValue("http://myprop1", "1")

    def test_object_property_reification(self):
        fact = autordf_py.Factory()
        autordf_py.Object.setFactory(fact)
        obj = autordf_py.Object("http://my/object")
        obj1 = autordf_py.Object("http://my/object1")
        obj2 = autordf_py.Object("http://my/object2")
        obj.setObject("http://myprop2", obj2)
        self.assertFalse(obj.reifiedObject("http://myprop2", obj2))
        obj.reifyObject("http://myprop1", obj1)
        self.assertTrue(obj.reifiedObject("http://myprop1", obj1))

        self.assertTrue(obj.getOptionalObject("http://myprop1"))
        self.assertEqual(obj1, obj.getObject("http://myprop1"))
        self.assertEqual([obj1], obj.getObjectList("http://myprop1", False))

        obj.removeObject("http://myprop1",  obj1)

    def test_object_property_reification2(self):
        fact = autordf_py.Factory()
        autordf_py.Object.setFactory(fact)
        obj = autordf_py.Object("http://my/object")
        obj1 = autordf_py.Object("http://my/object1")
        obj.setObject("http://myprop1", obj1)
        obj.reifyObject("http://myprop1", obj1)
        self.assertEqual(4, fact.findSize())

    def test_object_property_unreification(self):
        fact = autordf_py.Factory()
        autordf_py.Object.setFactory(fact)
        obj = autordf_py.Object("http://my/object")
        obj.unReifyObject("http://unexistingprop", autordf_py.Object("http://unexisting"))
        obj1 = autordf_py.Object("http://my/object1")
        reified = obj.reifyObject("http://myprop1", obj1)
        reified.addPropertyValue("http://thirdpartyprop", "thirdpartypropvalue", False)
        self.assertRaises(autordf_py.CannotUnreify, obj.unReifyObject, "http://myprop1", obj1)
        reified.removePropertyValue("http://thirdpartyprop", "thirdpartypropvalue")
        obj.unReifyObject("http://myprop1", obj1)

    def test_data_property_ordering_vector(self):
        fact = autordf_py.Factory()
        autordf_py.Object.setFactory(fact)
        obj = autordf_py.Object("http://my/object")
        pvv = [autordf_py.PropertyValue("stringval1"),autordf_py.PropertyValue("stringval2")]
        obj.setPropertyValueList("http://prop1", pvv, True)
        obj.getPropertyValueList("http://prop1", True)

    def test_data_property_ordering_add(self):
        fact = autordf_py.Factory()
        autordf_py.Object.setFactory(fact)
        obj = autordf_py.Object("http://my/object")
        obj.setPropertyValue("http://prop1", "stringval1")
        obj.addPropertyValue("http://prop1", "stringval2", True)
        self.assertRaises(autordf_py.CannotPreserveOrder, obj.getPropertyValueList, "http://prop1", True)

    def test_data_object_ordering_vector(self):
        fact = autordf_py.Factory()
        autordf_py.Object.setFactory(fact)
        obj = autordf_py.Object("http://my/object")
        pvv = [autordf_py.Object("http://object1"), autordf_py.Object("http://object1")]
        obj.setObjectList("http://prop1", pvv, True)
        read = obj.getObjectList("http://prop1", True)
        read[0], read[1] = read[1], read[0]
        obj.setObjectList("http://prop1", read, True)
        read2 = obj.getObjectList("http://prop1", True)
        self.assertEqual(2, len(read2))
        self.assertEqual(pvv[0], read2[1])
        self.assertEqual(pvv[1], read2[0])

    def test_data_object_ordering_add(self):
        fact = autordf_py.Factory()
        autordf_py.Object.setFactory(fact)
        obj = autordf_py.Object("http://my/object")
        obj.setObject("http://prop1", autordf_py.Object("http://object1"))
        obj.addObject("http://prop1", autordf_py.Object("http://object2"), True)
        self.assertRaises(autordf_py.CannotPreserveOrder, obj.getObjectList, "http://prop1", True)

    def test_object_type(self):
        fact = autordf_py.Factory()
        autordf_py.Object.setFactory(fact)
        obj = autordf_py.Object("http://my/object", "http://myobjecttype")
        obj.writeRdfType()
        self.assertTrue(obj.isA("http://myobjecttype"))
        self.assertEqual("http://myobjecttype", obj.getTypes()[0])

    def test_find_sources(self):
        fact = autordf_py.Factory()
        autordf_py.Object.setFactory(fact)
        fact.loadFromFile(os.path.dirname(os.path.abspath(__file__)) + "/foafExample.ttl", "http://xmlns.com/foaf/0.1/")
        objs = autordf_py.Object.findByType("http://xmlns.com/foaf/0.1/Person")
        for p in objs:
            if p.getPropertyValue("http://xmlns.com/foaf/0.1/name") == "Jimmy Wales":
                jimmy = p
            elif p.getPropertyValue("http://xmlns.com/foaf/0.1/name") == "Angela Beesley":
                angela = p
        sources = angela.findSources()
        self.assertTrue(angela not in sources)
        self.assertTrue(jimmy in sources)

    def test_find_targets(self):
        fact = autordf_py.Factory()
        autordf_py.Object.setFactory(fact)
        fact.loadFromFile(os.path.dirname(os.path.abspath(__file__)) + "/foafExample.ttl", "http://xmlns.com/foaf/0.1/")
        objs = autordf_py.Object.findByType("http://xmlns.com/foaf/0.1/Person")
        for p in objs:
            if p.getPropertyValue("http://xmlns.com/foaf/0.1/name") == "Jimmy Wales":
                jimmy = p
                break
        targets = jimmy.findTargets()
        self.assertTrue(jimmy not in targets)
        known = jimmy.getObject("http://xmlns.com/foaf/0.1/knows")
        self.assertTrue(known in targets)

    def test_sources_and_targets_reification(self):
        fact = autordf_py.Factory()
        autordf_py.Object.setFactory(fact)
        obja = autordf_py.Object("http://objecta")
        objb = autordf_py.Object("http://objectb")
        obja.setObject("http://myprop1", objb)
        reified = obja.reifyObject("http://myprop1", objb)
        self.assertEqual(1, len(objb.findSources()))
        self.assertEqual(obja, objb.findSources()[0])
        self.assertEqual(1, len(obja.findTargets()))
        self.assertEqual(objb, obja.findTargets()[0])

    def test_clone_recursive_stop_at_ressources(self):
        fact = autordf_py.Factory()
        autordf_py.Object.setFactory(fact)
        fact.loadFromFile(os.path.dirname(os.path.abspath(__file__)) + "/foafExample.ttl", "http://xmlns.com/foaf/0.1/")
        objs = autordf_py.Object.findByType("http://xmlns.com/foaf/0.1/Person")
        for p in objs:
            if p.getPropertyValue("http://xmlns.com/foaf/0.1/name") == "Jimmy Wales":
                jimmy = p
                break
        clone = jimmy.cloneRecursiveStopAtResources("http://jimmycricket.com/me2")
        known = jimmy.getObject("http://xmlns.com/foaf/0.1/knows")
        known2 = clone.getObject("http://xmlns.com/foaf/0.1/knows")
        self.assertEqual(known, known2)
        objs = autordf_py.Object.findByType("http://xmlns.com/foaf/0.1/Person")
        count = sum(o.getPropertyValue("http://xmlns.com/foaf/0.1/name") == "Jimmy Wales" for o in objs)
        self.assertEqual(count, 2)

    def test_clone_reification(self):
        fact = autordf_py.Factory()
        autordf_py.Object.setFactory(fact)
        obj = autordf_py.Object("http://my/object")
        obj.reifyPropertyValue("http://myprop1", "1")
        clone = obj.cloneRecursiveStopAtResources("http://my/object2")
        self.assertTrue(clone.reifiedPropertyValue("http://myprop1", "1"))

    def test_clone_reification_via_standard_clone(self):
        fact = autordf_py.Factory()
        autordf_py.Object.setFactory(fact)
        obj = autordf_py.Object("http://my/object")
        obj.reifyPropertyValue("http://myprop1", "1")
        clone = obj.clone("http://my/object2")
        self.assertTrue(clone.reifiedPropertyValue("http://myprop1", "1"))

    def test_remove_object_recursive(self):
        fact = autordf_py.Factory()
        autordf_py.Object.setFactory(fact)
        obj = autordf_py.Object("http://testns/myobject", "http://testns/type1")

        obj.addPropertyValue("http://property", "val1", False)
        self.assertEqual(2, fact.findSize())
        obj.addPropertyValue("http://property", "val2", False)
        self.assertEqual(3, fact.findSize())

        subobject = autordf_py.Object("http://testns/subobj")
        subobject.addPropertyValue("http://property", "val3", False)
        obj.addObject("http://subobject", autordf_py.Object("http://testns/subobject"), False)
        self.assertEqual(5, fact.findSize())

        subobject2 = autordf_py.Object("http://subobj/bnode")
        bnode = obj.reifyObject("http://testns/bnode", subobject2)
        bnode.addPropertyValue("http://property", "val4", False)
        bnode.addObject("http://subobject", autordf_py.Object("http://bnode/subobject"), False)
        obj.addObject("http://testns/obj", bnode, False)
        self.assertEqual(12, fact.findSize())

        obj.remove(True)
        self.assertEqual(1, fact.findSize())

    def test_remove_object_recursive2(self):
        fact = autordf_py.Factory()
        autordf_py.Object.setFactory(fact)
        obj = autordf_py.Object("http://testns/myobject", "http://testns/type1")
        bnode1 = autordf_py.Object("", "http://testns/type2")
        bnode1.addPropertyValue("http://property", "val1", False)
        bnode1.addObject("http://subobject", autordf_py.Object("http://bnode1/subobject"), False)
        obj.addObject("http://subobject", bnode1, False)

        subobject2 = autordf_py.Object("http://testns/type3")
        bnode2 = bnode1.reifyObject("http://testns/reifiedobj", subobject2)
        bnode2.addPropertyValue("http://property", "val2", False)
        bnode2.addObject("http://subobject", autordf_py.Object("http://bnode2/subobject"), False)
        bnode1.addObject("http://subobject", bnode2, False)
        self.assertEqual(12, fact.findSize())
        obj.remove(True)
        self.assertEqual(0, fact.findSize())
