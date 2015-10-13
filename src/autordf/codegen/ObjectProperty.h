#ifndef AUTORDF_CODEGEN_OBJECTPROPERTY_H
#define AUTORDF_CODEGEN_OBJECTPROPERTY_H

#include <memory>
#include <string>
#include <map>
#include <iosfwd>

#include "Property.h"

namespace autordf {
namespace codegen {

class ObjectProperty : public Property {
public:

    // Returns class for this property, or nullptr if no class is registered
    std::shared_ptr<Klass> findClass() const;

    void generateDeclaration(std::ostream& ofs, const Klass& onClass) const;

    void generateDefinition(std::ostream& ofs, const Klass& onClass) const;

    // iri to Property map
    static std::map<std::string, std::shared_ptr<ObjectProperty> > uri2Ptr;

private:
    void generateDeclarationSetterForOne(std::ostream& ofs, const Klass& onClass) const;

    void generateDeclarationSetterForMany(std::ostream& ofs, const Klass& onClass) const;

    void generateDefinitionSetterForOne(std::ostream& ofs, const Klass& onClass) const;

    void generateDefinitionSetterForMany(std::ostream& ofs, const Klass& onClass) const;
};
}
}
#endif //AUTORDF_CODEGEN_OBJECTPROPERTY_H
