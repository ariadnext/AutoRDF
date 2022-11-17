{% set method = concat("set", capitalize(property.name)) %}
{% set comment = property.comment %}
{% set comment.extraComment = ["Sets the mandatory value for this property."] %}
{% set comment.param = "value value to set for this property, removing all other values" %}
{% include "../property_comment.tpl" %}
    {{ interfaceName }}& {{ method }}(const {{ cppType(property.type.value) }}& value) {
        object().setPropertyValue("{{ property.rdfName }}", {% if not property.type.undefined %}autordf::PropertyValue().set<autordf::cvt::RdfTypeEnum::{{ rdfType(property.type.value) }}>(value));
        {% else %}value);
        {% endif %}
        return *this;
    }
