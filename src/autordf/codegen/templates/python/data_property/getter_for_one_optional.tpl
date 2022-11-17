{% set method = concat("get", capitalize(property.name), "Optional") %}
{% set comment = property.comment %}
{% set comment.return = "the value if it is set, or None if it is not set." %}
{% include "../property_comment.tpl" %}
    def {{ method }}(self):
        return self.getOptionalPropertyValue("{{ property.rdfName }}")


{% set method = concat("get", capitalize(property.name)) %}
{% set comment = property.comment %}
{% set comment.return = "the value if it is set, or defaultval if it is not set." %}
{% include "../property_comment.tpl" %}
    def {{ method }}(self, defaultval):
        ptr = self.getOptionalPropertyValue("{{ property.rdfName }}")
        return autordf_py.PropertyValue(defaultval) if ptr == None else ptr
