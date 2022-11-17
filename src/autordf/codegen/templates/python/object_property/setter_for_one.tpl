{% set method = concat("set", capitalize(property.name)) %}
{% set comment = property.comment %}
{% set comment.extraComment = ["Sets the mandatory value for this property."] %}
{% set comment.param = "value value to set for this property, removing all other values" %}
{% include "../property_comment.tpl" %}
    def {{ method }}(self, value):
        self.setObject("{{ property.rdfName }}", value)
        return self
