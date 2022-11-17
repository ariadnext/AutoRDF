{% set method = concat("set", capitalize(property.name)) %}
{% if existsIn(property.comment, "label") or existsIn(property.comment, "comment") %}
    {% set comment = property.comment%}
{% else %}
    {% set comment = property.class.classComment %}
{% endif %}
{% set comment.extraComment = ["Sets the mandatory value for this property."] %}
{% set comment.param = "value value to set for this property, removing all other values" %}
{% include "../property_comment.tpl" %}
    {{ interfaceName }}& {{ method }}(const {{ property.class.fullInterfaceName }}& value);
