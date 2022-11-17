{% set method = concat("set", capitalize(property.name)) %}
{% set comment = property.comment %}
{% set comment.extraComment = ["Sets property to list of values"] %}
{% set comment.param = "values the list of values" %}
{% include "../property_comment.tpl" %}
    {{ interfaceName }}& {{ method }}(const {{ cppManyType(property.type.value ) }}& values){% if not property.type.undefined %};{% else %}{{ whitespace(1) }}{
        object().setPropertyValueList("{{ property.rdfName }}", values, {% if property.ordered %}true{% else %}false{% endif %});
        return *this;
    }
{% endif %}

{% set method = concat("add", capitalize(property.name)) %}
{% set comment = property.comment %}
{% set comment.extraComment = ["Adds a value to a property"] %}
{% set comment.param = "value the value to add" %}
{% include "../property_comment.tpl" %}
    {{ interfaceName }}& {{ method }}(const {{ cppType(property.type.value) }}& value) {
        object().addPropertyValue("{{ property.rdfName }}", {% if not property.type.undefined %}autordf::PropertyValue().set<autordf::cvt::RdfTypeEnum::{{ rdfType(property.type.value) }}>(value){% else %}value{% endif %}, {% if property.ordered %}true{% else %}false{% endif %});
        return *this;
    }
