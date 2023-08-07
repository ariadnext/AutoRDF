{% set method = concat("get", capitalize(property.name)) %}
{% set comment = property.comment %}
{% set comment.return = "the mandatory value for this property." %}
{% set comment.throw = "PropertyNotFound if value is not set in database" %}
{% include "../property_comment.tpl" %}
    def {{ method }}(self):
        from {{ property.class.fullClassName }} import {{ property.class.className }}
        return {{ property.class.className }}(other=self.getObject("{{ property.rdfName }}"))
