#pragma once

// This is auto generated code by AutoRDF, do not edit !

{% if hasOneOf %}
#include <array>
#include <tuple>
#include <ostream>
#include <optional>
{% endif %}
#include <autordf/Object.h>

{% for dep in dependencies %}
{% set namespaces = dep.packages %}
{% include "enter_namespace.tpl" %}
class {{ dep.className }};
class {{ dep.interfaceName }};
{% include "leave_namespace.tpl" %}
{% endfor %}
{% set namespaces = packages %}
{% include "enter_namespace.tpl" %}
class {{ className }};
{% include "leave_namespace.tpl" %}

{% include "enter_namespace.tpl" %}
{% set comment = classComment %}
{% include "comment.tpl" %}
class {{ interfaceName }} {
public:
    {% set internalIndent = 1 %}
    {% include "enter_internal.tpl" %}
    /**
     * IRI for rdfs type associated with this C++ Interface class
     */
    static const char* TYPEIRI;
    {% include "leave_internal.tpl" %}

{% if hasOneOf %}
    /**
     * @brief C++ Enum values mapping the owl instances restrictions for this class
     */
    enum Enum {
    {% for oneOf in oneOfValues %}
        {% set comment = oneOf.comment %}
        {% set comment.indent = 2 %}
        {% include "comment.tpl" %}
        {{ oneOf.name }},
    {% endfor %}
    };

    /**
     * @brief Returns the current object as an Enum
     *
     * This object is expected to be one the instance allowed by the owl:oneOf definition.
     * @return enum value
     * @throw InvalidEnum if the object we try to convert is not one of the instances defined by owl:oneOf
     */
    Enum asEnum() const;

    /**
     * @brief Converts an enum value to a pretty string
     *
     * @param en enum value
     * @return enum value converted as string
     */
    static std::string enumString(Enum en);

    /**
     * @brief Converts an enum as a string to an enum
     *
     * @param enumString enum as string
     * @return enum value
     * @throw InvalidEnum if the string we try to convert is not one of the instances defined by owl:oneOf
     */
    static Enum enumFromString(const std::string& enumString);

    /**
     * @brief Converts current enum value to a pretty string
     *
     * @return current enum value converted as string
     */
    std::string enumString() const { return enumString(asEnum()); }

{% endif %}
{% for annotationProperty in annotationProperties %}
    {% set property = annotationProperty %}
    {% include "data_property/data_property_declaration.tpl" %}

{% endfor %}
{% for dataProperty in dataProperties %}
    {% set property = dataProperty %}
    {% include "data_property/data_property_declaration.tpl" %}

{% endfor %}
{% for objectProperty in objectProperties %}
    {% set property = objectProperty %}
    {% include "object_property/object_property_declaration.tpl" %}

{% endfor %}
    /**
     * @brief returns the object this interface object applies to
     */
    virtual autordf::Object& object() = 0;

    /**
     * @brief returns the object this interface object applies to
     */
    virtual const autordf::Object& object() const = 0;
{% if hasOneOf %}

private:
    typedef std::tuple<Enum, const char *, const char *> EnumArrayEntryType;
    typedef std::array<EnumArrayEntryType, {{ length(oneOfValues) }}> EnumArrayType;
    static const EnumArrayType ENUMARRAY;
    static const EnumArrayEntryType& enumVal2Entry(Enum en);

protected:
    {% set internalIndent = 1 %}
    {% include "enter_internal.tpl" %}
    {{ interfaceName }}() {}
    {{ interfaceName }}( const {{ interfaceName }}&) {}
    static std::string enumIri(Enum en);
    {% include "leave_internal.tpl" %}
{% endif %}
};
{% include "leave_namespace.tpl" %}
{% if hasOneOf %}

/**
 * Dumps string representation of Enumerated type
 */
inline std::ostream& operator<<(std::ostream& os, {{ fullInterfaceName }}::Enum val) {
    os << {{ fullInterfaceName }}::enumString(val);
    return os;
}
{% endif %}
