#pragma once

#include "../Klass.h"

namespace autordf {
namespace codegen {
namespace cpp {
class CppKlass : public Klass {
public:
    CppKlass(const ontology::Klass& ontology, inja::Environment& renderer) : Klass(ontology, renderer) {}
    explicit CppKlass(const ontology::Klass& ontology) : Klass(ontology) {}

    void generate() const override;

    void setSeparateHeaders(bool state);

private:
    bool _separateHeaders = false;

    std::unique_ptr<Klass> buildDependency(const ontology::Klass& ontology) const override;
};
}
}
}
