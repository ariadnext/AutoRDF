const autordf::Uri {{ interfaceName }}::{{ property.name }}ObjectPropertyIri = "{{ property.rdfName }}";

{% if property.maxCardinality <= 1 %}
    {% if property.minCardinality > 0 %}
{{ property.class.fullClassName }} {{ interfaceName }}::{{ property.name }}() const {
    return object().getObject("{{ property.rdfName }}").as<{{ property.class.fullClassName }}>();
}
    {% else %}
std::shared_ptr<{{ property.class.fullClassName }}> {{ interfaceName }}::{{ property.name }}Optional() const {
    auto result = object().getOptionalObject("{{ property.rdfName }}");
    return result ? std::make_shared<{{ property.class.fullClassName }}>(*result) : nullptr;
}
    {% endif %}
{% endif %}
{% if property.maxCardinality > 1%}
std::vector<{{ property.class.fullClassName }}> {{ interfaceName }}::{{ property.name }}List() const {
    return object().getObjectListImpl<{{ property.class.fullClassName }}>("{{ property.rdfName }}", {% if property.ordered %}true{% else %}false{% endif %});
}

    {% include "setter_for_many_definition.tpl"%}
{% endif %}

{% include "setter_for_one_definition.tpl"%}
{% if property.minCardinality != property.maxCardinality %}

    {% include "remover_definition.tpl"%}
{% endif %}
