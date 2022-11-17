{% set internalIndent = 0 %}
{% include "enter_internal.tpl" %}
#include <{{ fullPackagePath }}/{{ interfaceName }}.h>

// This is auto generated code by AutoRDF, do not edit !

#include <sstream>
#include <autordf/Exception.h>

{% for dep in dependencies %}
#include <{{ dep.fullPackagePath }}/{{ dep.className }}.h>
{% endfor %}
#include <{{ fullPackagePath }}/{{ className }}.h>

{% set namespaces = packages %}
{% include "enter_namespace.tpl" %}
{% if hasOneOf %}
const {{ className }}::EnumArrayType {{ interfaceName }}::ENUMARRAY = {
{
    {% for oneOf in oneOfValues %}
    std::make_tuple({{ interfaceName }}::{{ oneOf.name }}, "{{ oneOf.rdfName }}", "{{ oneOf.name }}"),
    {% endfor %}
}
};

const {{ interfaceName }}::EnumArrayEntryType& {{ interfaceName }}::enumVal2Entry(Enum enumVal) {
    for (auto const& enumItem: ENUMARRAY) {
        if ( enumVal == std::get<0>(enumItem) ) return enumItem;
    }
    std::stringstream ss;
    ss << "Enum value " << enumVal << " is not valid for for C++ enum {{ className }}";
    throw autordf::InvalidEnum(ss.str());
};

{{ interfaceName }}::Enum {{ interfaceName }}::asEnum() const {
    for (auto const& enumItem: ENUMARRAY) {
        if (object().iri() == std::get<1>(enumItem)) return std::get<0>(enumItem);
    }
    throw autordf::InvalidEnum(object().iri() + " is not a valid individual for owl:oneOf type {{ className }}");
}

std::string {{ interfaceName }}::enumIri(Enum enumVal) {
    return std::get<1>(enumVal2Entry(enumVal));
}

std::string {{ interfaceName }}::enumString(Enum enumVal) {
    return std::get<2>(enumVal2Entry(enumVal));
}

{{ interfaceName }}::Enum {{ interfaceName }}::enumFromString(const std::string& enumString) {
    for (auto const& enumItem: ENUMARRAY) {
        if (enumString == std::get<2>(enumItem)) return std::get<0>(enumItem);
    }
    throw autordf::InvalidEnum(enumString + " is not a valid individual for owl:oneOf type {{ className }}");
}
{% endif %}
// This type {{ fullInterfaceName }} has IRI {{ rdfName }}
const char* {{ interfaceName }}::TYPEIRI = "{{ rdfName }}";

{% for annotationProperty in annotationProperties %}
    {% set property = annotationProperty %}
    {% include "data_property/data_property_definition.tpl" %}

{% endfor %}
{% for dataProperty in dataProperties %}
    {% set property = dataProperty %}
    {% include "data_property/data_property_definition.tpl" %}

{% endfor %}
{% for objectProperty in objectProperties %}
    {% set property = objectProperty %}
    {% include "object_property/object_property_definition.tpl" %}

{% endfor %}
{% include "leave_namespace.tpl" %}
{% include "leave_internal.tpl" %}
