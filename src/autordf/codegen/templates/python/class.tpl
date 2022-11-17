"""@cond internal"""
"""This is auto generated code by AutoRDF, do not edit !"""

import autordf_py
from {{ fullInterfaceName }} import {{ interfaceName }}
{% for ancestor in ancestors %}
from {{ ancestor.fullInterfaceName }} import {{ ancestor.interfaceName }}
{% endfor %}

{% set comment = classComment %}
{% include "comment.tpl" %}
class {{ className }}({% for ancestor in ancestors %}{{ ancestor.interfaceName }}, {% endfor %}{{ interfaceName }}, autordf_py.Object):

    """
    @brief Creates new object
{% if hasOneOf %}
    @param enumVal if set, use the iri of the enum value to build the object
    This applies only to classes defines using the owl:oneOf paradigm
{% endif %}
    @param other if set, build the new object using the same underlying resource as the other object
    @param iri if iri empty, creates an anonymous (aka blank) object
    """
    def __init__(self, *args, **kwargs):
{% if hasOneOf %}
        if "enumVal" in kwargs:
            autordf_py.Object.__init__(self, enumIri(kwargs["enumVal"]))
{% endif %}
        {% if hasOneOf %}{{ indent(2) }}elif{% else %}{{ indent(2) }}if{% endif %}{{ whitespace(1) }}"other" in kwargs:
            autordf_py.Object.__init__(self, kwargs["other"])
        else:
            autordf_py.Object.__init__(self, kwargs.get("iri", ""), {{ interfaceName }}.TYPEIRI)

    """
    @brief Clone the object, to given iri
    Object is copied by duplicating all its properties values.
    @param iri if empty, creates an anonymous (aka blank) object.
    """
    def clone(self, iri = ""):
        return {{ className }}(other=autordf_py.Object.clone(self, iri))

    """
    @brief This method {{ fullClassName}}.find returns all resources of type {{ rdfName }}
    """
    @staticmethod
    def find():
        return [{{ className }}(other=found) for found in autordf_py.Object.findByType({{ interfaceName }}.TYPEIRI)]

"""@endcond internal"""
