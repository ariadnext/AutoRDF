    """Full iri for {{ property.name }} object property."""
    {{ property.name }}ObjectPropertyIri = "{{ property.rdfName }}"

{% if property.maxCardinality <= 1 %}
    {% if property.minCardinality > 0 %}
        {% include "getter_for_one_mandatory.tpl"%}
    {% else %}
        {% include "getter_for_one_optional.tpl"%}
    {% endif %}
{% endif %}
{% if property.maxCardinality > 1 %}
    {% include "getter_for_many.tpl"%}

    {% include "setter_for_many.tpl"%}
{% endif %}

{% include "setter_for_one.tpl"%}
{% if property.minCardinality != property.maxCardinality %}

    {% include "remover.tpl"%}
{% endif %}
