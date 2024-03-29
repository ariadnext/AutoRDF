{% set method = concat("get", capitalize(property.name), "List") %}
{% set comment = property.comment %}
{% set comment.return = "the list of typed objects. List can be empty if not values are set in database" %}
{% include "../property_comment.tpl" %}
    def {{ method}}(self):
        from {{ property.class.fullClassName }} import {{ property.class.className }}
        return [{{ property.class.className }}(other=elem) for elem in self.getObjectList("{{ property.rdfName }}", {% if property.ordered %}True{% else %}False{% endif %})]
