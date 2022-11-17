#pragma once

// This is auto generated code by AutoRDF, do not edit !

#include <set>
#include <iosfwd>
#include <autordf/Object.h>
#include <{{ fullPackagePath }}/{{ interfaceName }}.h>
{% for ancestor in ancestors %}
#include <{{ ancestor.fullPackagePath }}/{{ ancestor.interfaceName }}.h>
{% endfor %}

{% set namespaces = packages %}
{% include "enter_namespace.tpl" %}
{% set comment = classComment %}
{% include "comment.tpl" %}
class {{ className }}: public autordf::Object{%for ancestor in ancestors %}, public {{ ancestor.fullInterfaceName }}{% endfor %}, public {{ interfaceName }} {
public:
    /**
     * @brief Creates new object, to given iri.
     *
     * If iri empty, creates an anonymous (aka blank) object
     */
    explicit {{ className }}(const std::string& iri = "");
{% if hasOneOf %}

    /**
     * @brief Load enum from RDF model, from given C++ Type enum.
     *
     * This applies only to classes defines using the owl:oneOf paradigm
     */
    explicit {{ className }}({{ interfaceName }}::Enum enumVal);
{% endif %}

    /**
     * @brief Build us using the same underlying resource as the other object
     */
    explicit {{ className }}(const Object& other);

    /**
     * @brief Default destructor
     */
    virtual ~{{ className }}() = default;

    /**
     * @brief Clone the object, to given iri
     *
     * Object is copied by duplicating all it properties values.
     * @param iri if empty, creates an anonymous (aka blank) object.
     */
    {{ className }} clone(const autordf::Uri& iri = "") const {
        return Object::clone(iri).as<{{ className }}>();
    }

    /**
     * @brief This method {{ fullClassName }}::find returns all resources of type {{ rdfName }}
     */
    static std::vector<{{ className }}> find();

{% for annotationKey in annotationKeys %}
    {% set key = annotationKey %}
    {% include "data_property/key_declaration.tpl" %}

{% endfor %}
{% for dataKey in dataKeys %}
    {% set key = dataKey %}
    {% include "data_property/key_declaration.tpl" %}

{% endfor %}
{% for objectKey in objectKeys %}
    {% set key = objectKey %}
    {% include "object_property/key_declaration.tpl" %}

{% endfor %}
private:
    Object& object() override { return *this; }
    const Object& object() const override { return *this; }
};
{% include "leave_namespace.tpl" %}
