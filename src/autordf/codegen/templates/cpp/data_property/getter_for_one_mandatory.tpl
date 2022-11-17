{% set method = property.name %}
{% set comment = property.comment %}
{% set comment.return = "the mandatory value for this property." %}
{% set comment.throw = "PropertyNotFound if value is not set in database" %}
{% include "../property_comment.tpl" %}
    {{ cppType(property.type.value) }} {{ method }}() const {
        return object().getPropertyValue("{{ property.rdfName }}"){% if property.type.undefined %};{% else %}.get<autordf::cvt::RdfTypeEnum::{{ rdfType(property.type.value) }}, {{ cppType(property.type.value) }}>();
        {% endif %}
    }
