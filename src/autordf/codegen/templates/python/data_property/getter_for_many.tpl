{% set method = concat("get", capitalize(property.name), "List") %}
{% set comment = property.comment %}
{% set comment.return = "the list of values. List can be empty if not values are set in database" %}
{% include "../property_comment.tpl" %}
    def {{ method }}(self):
        return self.getPropertyValueList("{{ property.rdfName }}", {% if property.ordered %}True{% else %}False{% endif %})
