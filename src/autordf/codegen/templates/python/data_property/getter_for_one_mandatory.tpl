{% set method = concat("get", capitalize(property.name)) %}
{% set comment = property.comment %}
{% set comment.return = "the mandatory value for this property." %}
{% set comment.throw = "PropertyNotFound if value is not set in database" %}
{% include "../property_comment.tpl" %}
    def {{ method }}(self):
        return self.getPropertyValue("{{ property.rdfName }}")
