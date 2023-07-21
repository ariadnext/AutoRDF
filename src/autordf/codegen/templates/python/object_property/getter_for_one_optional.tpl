{% set method = concat("get", capitalize(property.name), "Optional") %}
{% set comment = property.comment %}
{% set comment.return = "the value if it is set, or Nones if it is not set." %}
{% include "../property_comment.tpl" %}
    def {{ method }}(self):
        from {{ property.class.fullClassName }} import {{ property.class.className }}
        opt = self.getOptionalObject("{{ property.name }}")
        return None if opt == None else {{ property.class.fullClassName }}(other=opt)
