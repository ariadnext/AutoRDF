{{ interfaceName }}& {{ interfaceName }}::set{{ capitalize(property.name) }}List(const std::vector<{{ property.class.fullClassName }}>& values) {
    object().setObjectListImpl<{{ property.class.fullClassName }}>("{{ property.rdfName }}", values, {% if property.ordered %}true{% else %}false{% endif %});
    return *this;
}

{{ interfaceName }}& {{ interfaceName }}::add{{ capitalize(property.name) }}(const {{ property.class.fullInterfaceName }}& value) {
    object().addObject("{{ property.rdfName }}", value.object(), {% if property.ordered %}true{% else %}false{% endif %});
    return *this;
}
