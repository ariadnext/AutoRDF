    /**
     * @brief This method {{ fullClassName}}::findBy{{ capitalize(key.name) }} returns the only instance of {{ rdfName }} with property {{ key.rdfName }} set to given object.
     *
     * @param key value that uniquely identifies the expected object
     *
     * @throw DuplicateObject if more than one object have the same property value
     * @throw ObjectNotFound if no object has given property with value
     */
    static {{ className }} findBy{{ capitalize(key.name) }}(const {{ key.class.fullClassName }}& key) {
        return Object::findByKey("{{ key.rdfName }}", key).as<{{ className }}>();
    }
