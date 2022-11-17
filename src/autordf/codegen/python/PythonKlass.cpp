#include "PythonKlass.h"

#include "../Environment.h"

namespace autordf {
namespace codegen {
namespace python {
PythonKlass::PythonKlass(const ontology::Klass& ontology, inja::Environment& renderer) :
        Klass(ontology, renderer) {
    _packageSeparator = ".";
    _pathSeparator = ".";
}

PythonKlass::PythonKlass(const ontology::Klass& ontology) : Klass(ontology) {
    _packageSeparator = ".";
    _pathSeparator = ".";
}

void PythonKlass::generate() const {
    std::ofstream out;

    if (Environment::verbose) {
        std::cout << "Rendering template 'python/interface.tpl' to '" << packagePath() + "/" + interfaceName() + ".py'...";
    }
    Environment::createFile(packagePath() + "/" + interfaceName() + ".py", out);
    auto tpl = _renderer.parse_template("python/interface.tpl");
    _renderer.render_to(out, tpl, templateData());
    out.close();
    if (Environment::verbose) {
        std::cout << " Done." << std::endl;
    }


    if (Environment::verbose) {
        std::cout << "Rendering template 'python/class.tpl' to '" << packagePath() + "/" + className() + ".py'...";
    }
    Environment::createFile(packagePath() + "/" + className() + ".py", out);
    tpl = _renderer.parse_template("python/class.tpl");
    _renderer.render_to(out, tpl, templateData());
    out.close();
    if (Environment::verbose) {
        std::cout << " Done." << std::endl;
    }
}

std::unique_ptr<Klass> PythonKlass::buildDependency(const ontology::Klass& ontology) const {
    return std::unique_ptr<Klass>(new PythonKlass(ontology));
}
}
}
}
