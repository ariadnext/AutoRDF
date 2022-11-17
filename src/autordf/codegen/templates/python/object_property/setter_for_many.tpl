{% set method = concat("set", capitalize(property.name), "List") %}
{% set comment = property.comment %}
{% set comment.extraComment = ["Sets the values for this property."] %}
{% set comment.param = "values values to set for this property" %}
{% include "../property_comment.tpl" %}
    def {{ method }}(self, values):
        self.setObjectList("{{ property.rdfName }}", values, {% if property.ordered %}True{% else %}False{% endif %})
        return self

{% set method = concat("add", capitalize(property.name)) %}
{% set comment = property.comment %}
{% set comment.extraComment = ["Adds a value for this property."] %}
{% set comment.param = "value value to set for this property, removing all other values" %}
{% include "../property_comment.tpl" %}
    def {{ method }}(self, value):
        self.addObject("{{ property.rdfName }}", value, {% if property.ordered %}True{% else %}False{% endif %})
        return self
