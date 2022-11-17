    """
    @brief This method {{ fullClassName }}.findBy{{ capitalize(property.name) }} returns the only instance of {{ rdfName }} with property {{ property.rdfName }} set to given object.

    @param key value that uniquely identifies the expected object
    @throw DuplicateObject if more than one object have the same property value
    @throw ObjectNotFound if no object has given property with value
    """
    i@staticmethod
    def findBy{{ capitalize(property.name) }}(key):
        return {{ className }}(other=autordf_py.Object.findByKey("{{ property.rdfName }}", key))
