#ifndef AUTORDF_CODEGEN_OBJECTPROPERTY_H
#define AUTORDF_CODEGEN_OBJECTPROPERTY_H

#include <memory>
#include <string>
#include <map>
#include <iosfwd>

#include <autordf/ontology/ObjectProperty.h>
#include "RdfsEntity.h"

namespace autordf {

namespace codegen {

class Klass;

class ObjectProperty: public RdfsEntity {
    const ontology::ObjectProperty& _decorated;
public:
    ObjectProperty(const ontology::ObjectProperty& decorated) : RdfsEntity(decorated), _decorated(decorated) {}

    void generateDeclaration(std::ostream& ofs, const Klass& onClass) const;

    void generateDefinition(std::ostream& ofs, const Klass& onClass) const;

    void generateKeyDeclaration(std::ostream& ofs, const Klass& onClass) const;

private:
    /**
     * Gets the class for this property, when instanciated as class "onClass" attribute
     */
    Klass effectiveClass(const Klass& onClass) const;

    void generateDeclarationSetterForOne(std::ostream& ofs, const Klass& onClass) const;

    void generateDeclarationSetterForMany(std::ostream& ofs, const Klass& onClass) const;

    void generateRemoverDeclaration(std::ostream& ofs, const Klass& onClass) const;

    void generateDefinitionSetterForOne(std::ostream& ofs, const Klass& onClass) const;

    void generateDefinitionSetterForMany(std::ostream& ofs, const Klass& onClass) const;

    void generateRemoverDefinition(std::ostream& ofs, const Klass& onClass) const;

};
}
}
#endif //AUTORDF_CODEGEN_OBJECTPROPERTY_H
