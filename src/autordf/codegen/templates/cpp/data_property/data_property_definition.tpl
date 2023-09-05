const char* {{ interfaceName}}::{{ property.name }}DataPropertyIri = "{{ property.rdfName }}";
{% if not property.type.undefined %}
    {% if property.maxCardinality == 1 and property.minCardinality == 0 %}

std::shared_ptr<{{ cppType(property.type.value) }}> {{ interfaceName }}::{{property.name}}Optional() const {
    auto ptrval = object().getOptionalPropertyValue("{{ property.rdfName }}");
    return (ptrval ? std::shared_ptr<{{ cppType(property.type.value) }}>(new {{ cppType(property.type.value) }}(ptrval->get<autordf::cvt::RdfTypeEnum::{{ property.type.rdf }}, {{ cppType(property.type.value) }}>())) : nullptr);
}

std::optional<{{ cppType(property.type.value) }}> {{ interfaceName }}::{{property.name}}Opt() const {
    auto ptrval = object().getOptionalPropertyValue("{{ property.rdfName }}");
    return (ptrval ? std::optional<{{cppType(property.type.value) }}>(static_cast<{{ cppType(property.type.value) }}>(ptrval->get<autordf::cvt::RdfTypeEnum::{{ property.type.rdf }}, {{ cppType(property.type.value) }}>())) : std::nullopt);
}

{{ cppType(property.type.value) }} {{ interfaceName }}::{{property.name}}(const {{ cppType(property.type.value) }}& defaultval) const {
    auto ptrval = object().getOptionalPropertyValue("{{ property.rdfName }}");
    return (ptrval ? ptrval->get<autordf::cvt::RdfTypeEnum::{{ property.type.rdf }}, {{ cppType(property.type.value) }}>() : defaultval);
}
    {% endif %}
    {% if property.maxCardinality > 1 %}
{{ cppManyType(property.type.value) }} {{ interfaceName }}::{{property.name}}List() const {
    return object().getValueListImpl<autordf::cvt::RdfTypeEnum::{{ property.type.rdf }}, {{ cppType(property.type.value) }}>("{{ property.rdfName }}", {% if property.ordered %}true{% else %}false{% endif %});
}

{{ interfaceName }}& {{ interfaceName }}::set{{ capitalize(property.name) }}(const {{ cppManyType(property.type.value) }}& values) {
    object().setValueListImpl<autordf::cvt::RdfTypeEnum::{{ property.type.rdf }}>("{{ property.rdfName }}", values, {% if property.ordered %}true{% else %}false{% endif %});
    return *this;
}
    {% endif %}
{% endif %}
