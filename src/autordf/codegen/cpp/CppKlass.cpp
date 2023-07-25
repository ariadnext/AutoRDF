#include "CppKlass.h"

#include "../Environment.h"

namespace autordf {
namespace codegen {
namespace cpp {
void CppKlass::generate() const {
    std::ofstream out;
    std::string headerPrefixPath;
    if (_separateHeaders) {
        headerPrefixPath = "include/";
    }

    if (Environment::verbose) {
        std::cout << "Rendering template 'cpp/interface_declaration.tpl' to '" << headerPrefixPath << packagePath() + "/" + interfaceName() + ".h'...";
    }
    Environment::createFile(headerPrefixPath + packagePath() + "/" + interfaceName() + ".h", out);
    auto tpl = _renderer.parse_template("cpp/interface_declaration.tpl");
    _renderer.render_to(out, tpl, templateData());
    out.close();
    if (Environment::verbose) {
        std::cout << " Done." << std::endl;
    }


    if (Environment::verbose) {
        std::cout << "Rendering template 'cpp/interface_definition.tpl' to '" << packagePath() + "/" + interfaceName() + ".cpp'...";
    }
    Environment::createFile(packagePath() + "/" + interfaceName() + ".cpp", out);
    tpl = _renderer.parse_template("cpp/interface_definition.tpl");
    _renderer.render_to(out, tpl, templateData());
    out.close();
    if (Environment::verbose) {
        std::cout << " Done." << std::endl;
    }


    if (Environment::verbose) {
        std::cout << "Rendering template 'cpp/class_declaration.tpl' to '" << headerPrefixPath << packagePath() + "/" + className() + ".h'...";
    }
    Environment::createFile(headerPrefixPath + packagePath() + "/" + className() + ".h", out);
    tpl = _renderer.parse_template("cpp/class_declaration.tpl");
    _renderer.render_to(out, tpl, templateData());
    out.close();
    if (Environment::verbose) {
        std::cout << " Done." << std::endl;
    }


    if (Environment::verbose) {
        std::cout << "Rendering template 'cpp/class_definition.tpl' to '" << packagePath() + "/" + className() + ".cpp'...";
    }
    Environment::createFile(packagePath() + "/" + className() + ".cpp", out);
    tpl = _renderer.parse_template("cpp/class_definition.tpl");
    _renderer.render_to(out, tpl, templateData());
    out.close();
    if (Environment::verbose) {
        std::cout << " Done." << std::endl;
    }
}

void CppKlass::setSeparateHeaders(bool state) {
    _separateHeaders = state;
}

std::unique_ptr<Klass> CppKlass::buildDependency(const ontology::Klass& ontology) const {
    return std::unique_ptr<Klass>(new CppKlass(ontology));
}
}
}
}
