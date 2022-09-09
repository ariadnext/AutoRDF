#ifndef AUTORDF_CODEGEN_OBJECTPROPERTY_H
#define AUTORDF_CODEGEN_OBJECTPROPERTY_H

#include <memory>
#include <string>
#include <map>
#include <iosfwd>

#include <autordf/ontology/ObjectProperty.h>
#include "RdfsEntityPython.h"

namespace autordf {

namespace codegen {

class KlassPython;

class ObjectPropertyPython: public RdfsEntityPython {
    const ontology::ObjectProperty& _decorated;
public:
    ObjectPropertyPython(const ontology::ObjectProperty& decorated) : RdfsEntityPython(decorated), _decorated(decorated) {}

    void generate(std::ostream& ofs, const KlassPython& onClass) const;

    void generateKey(std::ostream& ofs, const KlassPython& onClass) const;

private:
    /**
     * Gets the class for this property, when instanciated as class "onClass" attribute
     */
    KlassPython effectiveClass(const KlassPython& onClass) const;

    void generateSetterForOne(std::ostream& ofs, const KlassPython& onClass) const;

    void generateSetterForMany(std::ostream& ofs, const KlassPython& onClass) const;

    void generateRemover(std::ostream& ofs, const KlassPython& onClass) const;

    std::string orderedBoolValue() const;
};
}
}
#endif //AUTORDF_CODEGEN_OBJECTPROPERTY_H
