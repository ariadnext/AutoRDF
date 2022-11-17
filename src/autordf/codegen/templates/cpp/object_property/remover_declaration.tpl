{% set method = concat("remove", capitalize(property.name)) %}
{% if existsIn(property.comment, "label") or existsIn(property.comment, "comment") %}
    {% set comment = property.comment%}
{% else %}
    {% set comment = property.class.classComment %}
{% endif %}
{% set comment.extraComment = ["Remove a value for this property."] %}
{% set comment.param = "obj value to remove for this property." %}
{% set comment.throw = "PropertyNotFound if propertyIRI has not obj as value" %}
{% include "../property_comment.tpl" %}
    {{ interfaceName }}& {{ method }}(const {{ property.class.fullInterfaceName }}& obj);
