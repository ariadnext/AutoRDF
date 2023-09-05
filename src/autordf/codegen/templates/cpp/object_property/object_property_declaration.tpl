    /**
     * Full iri for {{ property.fullName }} object property.
     */
    static const autordf::Uri {{ property.name }}ObjectPropertyIri;

    {% if property.maxCardinality <= 1 %}
        {% if property.minCardinality > 0 %}
            {% set method = property.name %}
            {% if existsIn(property.comment, "label") or existsIn(property.comment, "comment") %}
                {% set comment = property.comment%}
            {% else %}
                {% set comment = property.class.classComment %}
            {% endif %}
            {% set comment.return = "the mandatory instance" %}
            {% set comment.throw = "PropertyNotFound if object reference is not set" %}
            {% include "../property_comment.tpl" %}
    {{ property.class.fullClassName }} {{ method }}() const;
        {% else %}
            {% set method = concat(property.name, "Optional") %}
            {% if existsIn(property.comment, "label") or existsIn(property.comment, "comment") %}
                {% set comment = property.comment%}
            {% else %}
                {% set comment = property.class.classComment %}
            {% endif %}
            {% set comment.return = "the object instance if it is set, or nullptr if it is not set." %}
            {% include "../property_comment.tpl" %}
    std::shared_ptr<{{ property.class.fullClassName }}> {{ method }}() const;

            {% set method = concat(property.name, "Opt") %}
            {% if existsIn(property.comment, "label") or existsIn(property.comment, "comment") %}
                {% set comment = property.comment%}
            {% else %}
                {% set comment = property.class.classComment %}
            {% endif %}
            {% set comment.return = "the object instance if it is set, or nullopt if it is not set." %}
            {% include "../property_comment.tpl" %}
    std::optional<{{ property.class.fullClassName }}> {{ method }}() const;
        {% endif %}
    {% else %}
        {% set method = concat(property.name, "List") %}
        {% if existsIn(property.comment, "label") or existsIn(property.comment, "comment") %}
            {% set comment = property.comment%}
        {% else %}
            {% set comment = property.class.classComment %}
        {% endif %}
        {% set comment.return = "the list typed objects. List can be empty if not values are set in database" %}
        {% include "../property_comment.tpl" %}
    std::vector<{{ property.class.fullClassName }}> {{ method }}() const;

        {% include "setter_for_many_declaration.tpl" %}
    {% endif %}

    {% include "setter_for_one_declaration.tpl" %}
    {% if property.minCardinality != property.maxCardinality %}

        {% include "remover_declaration.tpl" %}
    {% endif %}
