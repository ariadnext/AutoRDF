{% set method = concat("set", capitalize(property.name), "List") %}
{% set comment = property.comment %}
{% set comment.extraComment = ["Sets property to list of values"] %}
{% set comment.param = "values the list of values" %}
{% include "../property_comment.tpl" %}
    def {{ method }}(self, values):
        self.setPropertyValueList("{{ property.rdfName }}", values, {% if property.ordered %}True{% else %}False{% endif %})
        return self

{% set method = concat("add", capitalize(property.name)) %}
{% set comment = property.comment %}
{% set comment.extraComment = ["Adds a value to a property"] %}
{% set comment.param = "value the value to add" %}
{% include "../property_comment.tpl" %}
    def {{ method }}(self, value):
        self.addPropertyValue("{{ property.rdfName }}", value, {% if property.ordered %}True{% else %}False{% endif %})
        return self
