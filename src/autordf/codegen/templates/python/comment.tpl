{% if not existsIn(comment, "indent") %}
    {% set comment.indent = 0 %}
{% endif %}
{{ indent(comment.indent) }}"""
{% if existsIn(comment, "label") %}
{{ indent(comment.indent) }}@brief {{ comment.label }}
{% endif %}
{% if existsIn(comment, "comment") %}
    {% for line in comment.comment %}
{{ indent(comment.indent) }}{{ line }}
    {% endfor %}
{% endif %}
{% if existsIn(comment, "seeAlso") %}
{{ indent(comment.indent) }}@see {{ comment.seeAlso }}
{% endif %}
{% if existsIn(comment, "isDefinedBy") %}
{{ indent(comment.indent) }}@see {{ comment.isDefinedBy }}
{% endif %}
{{ indent(comment.indent) }}"""
