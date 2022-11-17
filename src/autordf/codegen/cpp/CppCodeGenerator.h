#pragma once

#include "../CodeGenerator.h"

namespace autordf {
namespace codegen {
namespace cpp {
class CppCodeGenerator : public CodeGenerator {
public:
    CppCodeGenerator(Factory* factory, bool allInOne) : CodeGenerator(factory), _allInOne(allInOne) {};

protected:
    void runInternal(const ontology::Ontology& ontology, inja::Environment& renderer) override;

private:
    bool _allInOne;

    static std::string rdfTypeToCppType(int type);

    static std::string rdfTypeToCppManyType(int type);
};
}
}
}
