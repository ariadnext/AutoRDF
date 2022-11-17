{% set method = concat("remove", capitalize(property.name)) %}
{% set comment = property.comment %}
{% set comment.extraComment = ["Remove a value for this property."] %}
{% set comment.param = "value to remove for this property." %}
{% set comment.throw = "PropertyNotFound if propertyIRI has not obj as value" %}
{% include "../property_comment.tpl" %}
    {{ interfaceName }}& {{ method }}(const {{ cppType(property.type.value) }}& value) {
        object().removePropertyValue("{{ property.rdfName }}", {% if not property.type.undefined %}autordf::PropertyValue().set<autordf::cvt::RdfTypeEnum::{{ rdfType(property.type.value) }}>(value){% else %}value{% endif %});
        return *this;
    }
