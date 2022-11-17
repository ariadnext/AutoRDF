#pragma once

#include "../CodeGenerator.h"

namespace autordf {
namespace codegen {
namespace python {
class PythonCodeGenerator : public CodeGenerator {
public:
    explicit PythonCodeGenerator(Factory* factory) : CodeGenerator(factory) {};

protected:
    void runInternal(const ontology::Ontology& ontology, inja::Environment& renderer) override;
};
}
}
}
