#pragma once

#include <inja/inja.hpp>

#include "autordf/Factory.h"
#include "autordf/ontology/Ontology.h"

namespace autordf {
namespace codegen {
/**
 *
 */
class CodeGenerator {
public:
    explicit CodeGenerator(Factory* factory, const std::string& whitespaceChar = " ", const std::string& indentChar = "    ") :
        _factory(factory),
        _whitespaceChar(whitespaceChar),
        _indentChar(indentChar) {};

    virtual ~CodeGenerator() = default;

    void run();

protected:
    Factory* _factory;
    std::string _whitespaceChar;
    std::string _indentChar;

    virtual void runInternal(const ontology::Ontology& ontology, inja::Environment& renderer) = 0;
};
}
}
