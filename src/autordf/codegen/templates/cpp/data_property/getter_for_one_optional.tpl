{% set method = concat(property.name, "Optional") %}
{% set comment = property.comment %}
{% set comment.return = "the valueif it is set, or nullptr if it is not set." %}
{% include "../property_comment.tpl" %}
    std::shared_ptr<{{ cppType(property.type.value) }}> {{ method }}() const{% if not property.type.undefined %};{% else %}{{ whitespace(1) }}{
            auto val = object().getOptionalPropertyValue("{{ property.rdfName }}");
            return val ? std::make_shared<{{ cppType(property.type.value) }}>(*val) : nullptr;
    }
{% endif %}

{% set method = concat(property.name, "Opt") %}
{% set comment = property.comment %}
{% set comment.return = "the valueif it is set, or std::nullopt if it is not set." %}
{% include "../property_comment.tpl" %}
    std::optional<{{ cppType(property.type.value) }}> {{ method }}() const{% if not property.type.undefined %};{% else %}{{ whitespace(1) }}{
            return object().getOptionalPropertyValue("{{ property.rdfName }}");
    }
{% endif %}


{% set method = property.name %}
{% set comment = property.comment %}
{% set comment.return = "the valueif it is set, or defaultval if it is not set." %}
{% include "../property_comment.tpl" %}
    {{ cppType(property.type.value )}} {{ method }}(const {{ cppType(property.type.value) }}& defaultval) const{% if not property.type.undefined %};{% else %}{{ whitespace(1) }}{
        auto ptr = object().getOptionalPropertyValue("{{ property.rdfName }}");
        return (ptr ? *ptr : defaultval);
    }
{% endif %}
