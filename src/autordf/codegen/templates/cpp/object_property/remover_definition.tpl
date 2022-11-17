{{ interfaceName }}& {{ interfaceName }}::remove{{ capitalize(property.name) }}(const {{ property.class.fullInterfaceName}}& value) {
    object().removeObject("{{ property.rdfName }}", value.object());
    return *this;
}
