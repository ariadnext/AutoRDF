#include "PythonCodeGenerator.h"

#include "../Environment.h"

#include "PythonKlass.h"

namespace autordf {
namespace codegen {
namespace python {
void PythonCodeGenerator::runInternal(const ontology::Ontology& ontology, inja::Environment& renderer) {
    if (Environment::verbose) {
        std::cout << "Starting Python code generation" << std::endl;
    }

    std::vector<std::string> alreadyCreated;
    for (auto const& klassMapItem: ontology.classUri2Ptr()) {
        auto klass = PythonKlass(*klassMapItem.second, renderer);
        klass.buildTemplateData();

        // created directory if needed
        Environment::createDirectory(klass.packagePath());

        if (Environment::verbose) {
            std::cout << "Generating class '" << klass.className() << "'..." << std::endl;
        }
        klass.generate();
        if (Environment::verbose) {
            std::cout << "Generation done." << std::endl;
        }

        if (Environment::verbose) {
            std::cout << "Adding class to __init__.py" << std::endl;
        }
        std::ofstream out;
        auto isCreated = std::find(alreadyCreated.begin(), alreadyCreated.end(), klass.packagePath()) != alreadyCreated.end();
        Environment::createFile(klass.packagePath() + "/__init__.py", out, isCreated);
        auto tpl = renderer.parse_template("python/init.tpl");
        nlohmann::json data;
        data["className"] = klass.className();
        data["interfaceName"] = klass.interfaceName();
        data["module"] = klass.fullPackageName();
        renderer.render_to(out, tpl, data);
        alreadyCreated.push_back(klass.packagePath());
    }
}
}
}
}
