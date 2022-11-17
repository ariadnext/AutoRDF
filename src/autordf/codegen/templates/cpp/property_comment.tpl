{% if comment.isDoxygen %}
    /**
{% else %}
    /*
{% endif %}
{% if existsIn(comment, "label") %}
     * @brief {{ comment.label }}
     *
{% endif %}
{% if existsIn(comment, "comment") %}
    {% for line in comment.comment %}
     * {{ line }}
    {% endfor %}
{% endif %}
{% if existsIn(comment, "seeAlso") %}
     * @see {{ comment.seeAlso }}
{% endif %}
{% if existsIn(comment, "isDefinedBy") %}
     * @see {{ comment.isDefinedBy }}
{% endif %}
{% if existsIn(comment, "extraComment") %}
    {% for line in comment.extraComment %}
     * {{ line }}
    {% endfor %}
{% endif %}
{% if existsIn(comment, "param") %}
     * @param {{ comment.param }}
{% endif %}
{% if existsIn(comment, "return") %}
     * @return {{ comment.return }}
{% endif %}
{% if existsIn(comment, "throw") %}
     * @throw {{ comment.throw }}
{% endif %}
     * This method {{ fullClassName }}::{{ method }} uses {{ property.rdfName }} rdfs/owl property
     */
