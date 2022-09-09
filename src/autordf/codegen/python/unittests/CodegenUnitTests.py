import unittest
import example.ex
import autordf_py

class CodegenUnitTests(unittest.TestCase):
    def test_bool_properties(self):
        fact = autordf_py.Factory()
        autordf_py.Object.setFactory(fact)
        objA = example.ex.ObjectA()
        objA.setBoolA("true")
        self.assertTrue(objA.getBoolA().get())

    def test_remove_property(self):
        fact = autordf_py.Factory()
        autordf_py.Object.setFactory(fact)
        objA = example.ex.ObjectA()
        objA.setBoolA("true")
        objA.removeBoolA("true")
        with self.assertRaises(autordf_py.PropertyNotFound):
            objA.getBoolA()

    def test_inheritance(self):
        fact = autordf_py.Factory()
        autordf_py.Object.setFactory(fact)
        objA = example.ex.InheritsA()
        objA.setBoolA("true") # small method works
        self.assertTrue(objA.getBoolA().get())

    def test_add_object_property(self):
        fact = autordf_py.Factory()
        autordf_py.Object.setFactory(fact)
        objB = example.ex.ObjectB()
        objA = example.ex.ObjectA()
        objA.addObjB(objB)
        self.assertEqual(objA.getObjBList()[0], objB)

    def test_optional(self):
        fact = autordf_py.Factory()
        autordf_py.Object.setFactory(fact)
        objB = example.ex.ObjectB()
        self.assertEqual(objB.getBoolAOptional(), None)
        self.assertFalse(objB.getBoolA("false").get())
        objB.setBoolA("true")
        self.assertTrue(objB.getBoolA("false").get())
        objB.removeBoolA("true")
        self.assertFalse(objB.getBoolA("false").get())

if __name__ == '__main__':
    unittest.main()
