#pragma once

#include <set>
#include <map>
#include <memory>
#include <string>

#include "RdfsEntityPython.h"

#include <autordf/ontology/Klass.h>

namespace autordf {
namespace codegen {

class KlassPython : public RdfsEntityPython {
    const ontology::Klass& _decorated;
public:
    KlassPython(const ontology::Klass& decorated) : RdfsEntityPython(decorated), _decorated(decorated) {}

    const ontology::Klass& decorated() const { return _decorated; }

    std::set<KlassPython> getClassDependencies() const;

    void generateInterface() const;

    void generate() const;

    KlassPython uri2Klass(const std::string& uri) const;

    bool operator<(const KlassPython& other) const {return this->decorated() < other.decorated(); }
private:
};

}
}
