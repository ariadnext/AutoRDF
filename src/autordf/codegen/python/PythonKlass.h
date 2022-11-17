#pragma once

#include "../Klass.h"

namespace autordf {
namespace codegen {
namespace python {
class PythonKlass: public Klass {
public:
    PythonKlass(const ontology::Klass& ontology, inja::Environment& renderer);
    explicit PythonKlass(const ontology::Klass& ontology);

    void generate() const override;

private:
    std::unique_ptr<Klass> buildDependency(const ontology::Klass& ontology) const override;
};
}
}
}
