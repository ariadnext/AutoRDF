{% set method = concat("remove", capitalize(property.name)) %}
{% set comment = property.comment %}
{% set comment.extraComment = ["Remove a value for this property."] %}
{% set comment.param = "value to remove for this property." %}
{% set comment.throw = "PropertyNotFound if propertyIRI has not obj as value" %}
{% include "../property_comment.tpl" %}
    def {{ method }}(self, value):
        self.removePropertyValue("{{ property.rdfName }}", value)
        return self
