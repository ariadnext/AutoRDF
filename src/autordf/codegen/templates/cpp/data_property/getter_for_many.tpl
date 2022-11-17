{% set method = concat(property.name, "List") %}
{% set comment = property.comment %}
{% set comment.return = "the list of values. List can be empty if not values are set in database" %}
{% include "../property_comment.tpl" %}
    {{ cppManyType(property.type.value) }} {{ method }}() const{% if not property.type.undefined %};{% else %}{{ whitespace(1) }}{
{% endif %}
{% if property.type.undefined %}
        return object().getPropertyValueList("{{ property.rdfName }}", {% if property.ordered %}true{% else %}false{% endif %});
    }
{% endif %}
