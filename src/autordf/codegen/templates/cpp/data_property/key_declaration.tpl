    /**
     * @brief This method {{ fullClassName }}::findBy{{ capitalize(key.name) }} returns the only instance of {{ rdfName }} with property {{ key.rdfName }} set to given value.
     *
     * @param key value that uniquely identifies the expected object
     *
     * @throw DuplicateObject if more than one object have the same property value
     * @throw ObjectNotFound if no object has given property with value
     */
    static {{ className }} findBy{{ capitalize(key.name) }}(const autordf::PropertyValue& key) {
        return Object::findByKey("{{ key.rdfName}}", key).as<{{ className }}>();
    }
{% if not key.type.undefined %}

    /**
     * @brief This method {{ fullClassName }}::findBy{{ capitalize(key.name) }} returns the only instance of {{ rdfName }} with property {{ key.rdfName }} set to given value.
     *
     * @param key value that uniquely identifies the expected object
     *
     * @throw DuplicateObject if more than one object have the same property value
     * @throw ObjectNotFound if no object has given property with value
     */
    static {{ className }} findBy{{ capitalize(key.name) }}(const {{ cppType(key.type.value) }}& key) {
        return findBy{{ capitalize(key.name) }}(autordf::PropertyValue().set<autordf::cvt::RdfTypeEnum::{{ key.type.rdf }}>(key));
    }
{% endif %}
