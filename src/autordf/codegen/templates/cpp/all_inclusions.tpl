#pragma once

// This is auto generated code by AutoRDF, do not edit !

{% for class in classes %}
#include <{{ class.fullPackagePath }}/{{ class.className }}.h>
{% endfor %}
