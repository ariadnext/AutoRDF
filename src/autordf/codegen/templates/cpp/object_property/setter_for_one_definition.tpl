{{ interfaceName }}& {{ interfaceName }}::set{{ capitalize(property.name) }}(const {{ property.class.fullInterfaceName }}& value) {
    object().setObject("{{ property.rdfName }}", value.object());
    return *this;
}
