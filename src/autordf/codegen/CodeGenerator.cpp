#include "CodeGenerator.h"

#include "Environment.h"

namespace autordf {
namespace codegen {
void CodeGenerator::run() {
    ontology::Ontology ontology = {_factory, Environment::verbose};

    inja::Environment renderer {Environment::tpldir, Environment::outdir};

    renderer.set_trim_blocks(true);
    renderer.set_lstrip_blocks(true);

    renderer.add_callback("concat", [](inja::Arguments& args) {
        std::string result;
        for (auto const& arg : args) {
            result += arg->get<std::string>();
        }
        return result;
    });

    renderer.add_callback("capitalize", 1, [](inja::Arguments& args) {
        auto result = args[0]->get<std::string>();

        if (!result.empty()) {
            result[0] = toupper(result[0]);
        }

        return result;
    });

    renderer.add_callback("rdfType", 1, [](inja::Arguments& args) {
        auto type = args[0]->get<int>();

        return cvt::rdfTypeEnumString(cvt::RdfTypeEnum(type));
    });

    renderer.add_callback("whitespace", 1, [&](inja::Arguments& args) {
        auto nbSpace = args[0]->get<int>();
        std::string result;

        for (int i = 0; i < nbSpace; ++i) {
            result += _whitespaceChar;
        }

        return result;
    });

    renderer.add_callback("indent", 1, [&](inja::Arguments& args) {
        auto nbIndent = args[0]->get<int>();
        std::string result;

        for (int i = 0; i < nbIndent; ++i) {
            result += _indentChar;
        }

        return result;
    });

    runInternal(ontology, renderer);
}
}
}
