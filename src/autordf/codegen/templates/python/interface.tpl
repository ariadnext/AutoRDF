"""@cond internal"""
"""This is auto generated code by AutoRDF, do not edit !"""

import autordf_py
{% if hasOneOf %}
from enum import Enum
{% endif %}
{% for dep in dependencies %}
import {{ dep.fullClassName }}
{% endfor %}


class {{ interfaceName }}(autordf_py.Object):

{% if hasOneOf %}
    """
    @brief Enum values mapping the owl instances restrictions for this class
    """
    class InternalEnum(Enum):
    {% for oneOf in oneOfValues %}
        {% set comment = oneOf.comment %}
        {% set comment.indent = 2 %}
        {% include "comment.tpl" %}
        {{ oneOf.name }} = {{ loop.index1 }}
    {% endfor %}
    ENUMARRAY = [
    {% for oneOf in oneOfValues %}
        (InternalEnum.{{ oneOf.name }}, "{{ oneOf.rdfName }}", "{{ oneOf.name }}")
    {% endfor %}
    ]

    """
    @brief Returns the associated mappings (rdf name, pretty name) to an enum value
    """
    @classmethod
    def enumVal2Entry(cls, enumVal):
        for enumItem in cls.ENUMARRAY:
            if enumVal == enumItem[0]:
                return enumItem
        exceptString = "Enum value %1 is not valid for python enum {{ className }}".format(enumVal)
        raise autordf_py.InvalidEnum(exceptString)

    """
    @brief Returns the current object as an Enum

    This object is expected to be one the instance allowed by the owl:oneOf definition.
    @return enum value
    @throw InvalidEnum if the object we try to convert is not one of the instances defined by owl:oneOf
    """
    def asEnum(self):
        for enumItem in self.ENUMARRAY:
            if self.iri() == enumItem[1]:
                return enumItem[0]
        raise autordf_py.InvalidEnum(self.iri() + " is not a valid individual for owl:oneOf type {{ className}}")

    @classmethod
    def enumIri(cls, enumVal):
        return cls.enumVal2Entry(enumVal)[1]

    """
    @brief Converts an enum value to a pretty string

    @param enumVal enum value
    @return enum value converted as string
    """
    @classmethod"
    def enumString(cls, enumVal):
        return cls.enumVal2Entry(enumVal)[2]

    """
    @brief Converts an enum as a string to an enum

    @param enumString enum as string
    @return enum value
    @throw InvalidEnum if the string we try to convert is not one of the instances defined by owl:oneOf
    """
    @classmethod
    def enumFromString(cls, enumString):
        for enumItem in cls.ENUMARRAY:
            if enumString == enumItem[2]:
                return enumItem[0]
        raise autordf_py.InvalidEnum(enumString + " is not a valid individual for owl:oneOf type {{ className }}")

    """
    @brief Converts current enum value to a pretty string

    @return current enum value converted as string
    """
    def enumStringFromSelf(self):
        return self.enumString(self.asEnum())

{% endif %}
    """This type {{ fullInterfaceName }} has IRI {{ rdfName }}"""
    TYPEIRI = "{{ rdfName }}"

{% for annotationKey in annotationKeys %}
    {% set key = annotationKey %}
    {% include "data_property/key.tpl" %}

{% endfor %}
{% for dataKey in dataKeys %}
    {% set key = dataKey %}
    {% include "data_property/key.tpl" %}

{% endfor %}
{% for objectKey in objectKeys %}
    {% set key = objectKey %}
    {% include "object_property/key.tpl" %}

{% endfor %}
{% for annotationProperty in annotationProperties %}
    {% set property = annotationProperty %}
    {% include "data_property/data_property.tpl" %}

{% endfor %}
{% for dataProperty in dataProperties %}
    {% set property = dataProperty %}
    {% include "data_property/data_property.tpl" %}

{% endfor %}
{% for objectProperty in objectProperties %}
    {% set property = objectProperty %}
    {% include "object_property/object_property.tpl" %}

{% endfor %}
"""@endcond internal"""
