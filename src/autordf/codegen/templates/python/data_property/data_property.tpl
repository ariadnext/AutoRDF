    """Full iri for {{ property.name }} data property."""
    {{ property.name }}DataPropertyIri = "{{ property.rdfName }}"

{% if property.maxCardinality <= 1 %}
    {% if property.minCardinality > 0 %}
        {% include "getter_for_one_mandatory.tpl"%}
    {% else %}
        {% include "getter_for_one_optional.tpl"%}
    {% endif %}

    {% include "setter_for_one.tpl"%}
{% endif %}
{% if property.maxCardinality > 1 %}
    {% include "getter_for_many.tpl"%}

    {% include "setter_for_one.tpl"%}

    {% include "setter_for_many.tpl"%}
{% endif %}
{% if property.minCardinality != property.maxCardinality %}

    {% include "remover.tpl"%}
{% endif %}
