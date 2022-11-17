{% set internalIndent = 0 %}
{% include "enter_internal.tpl" %}
#include <{{ fullPackagePath }}/{{ className }}.h>

// This is auto generated code by AutoRDF, do not edit !

{% set namespaces = packages %}
{% include "enter_namespace.tpl" %}
{{ className }}::{{ className }}(const std::string& iri) : autordf::Object(iri, {{ interfaceName }}::TYPEIRI) {}
{% if hasOneOf %}

{{ className }}::{{ className }}({{ interfaceName }}::Enum enumVal) : autordf::Object(enumIri(enumVal)) {}
{% endif %}

{{ className }}::{{ className }}(const Object& other) : autordf::Object(other) {}

std::vector<{{ className }}> {{ className }}::find() {
    return findHelper<{{ className }}>({{ interfaceName }}::TYPEIRI);
}
{% include "leave_namespace.tpl" %}
{% include "leave_internal.tpl" %}
