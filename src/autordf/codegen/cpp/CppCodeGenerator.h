#pragma once

#include "../CodeGenerator.h"

namespace autordf {
namespace codegen {
namespace cpp {
class CppCodeGenerator : public CodeGenerator {
public:
    CppCodeGenerator(Factory* factory, bool allInOne, bool separateHeaders) :
        CodeGenerator(factory), _allInOne(allInOne), _separateHeaders(separateHeaders) {};

protected:
    void runInternal(const ontology::Ontology& ontology, inja::Environment& renderer) override;

private:
    bool _allInOne;
    bool _separateHeaders;

    static std::string rdfTypeToCppType(int type);

    static std::string rdfTypeToCppManyType(int type);
};
}
}
}
