#ifndef AUTORDF_CODEGEN_DATAPROPERTY_H
#define AUTORDF_CODEGEN_DATAPROPERTY_H

#include <map>
#include <memory>
#include <ostream>

#include <autordf/ontology/DataProperty.h>
#include "RdfsEntity.h"

namespace autordf {

namespace codegen {

class Klass;

class DataProperty : public RdfsEntity {
    const ontology::DataProperty& _decorated;
public:
    DataProperty(const ontology::DataProperty& decorated) : RdfsEntity(decorated), _decorated(decorated) {}

    void generateDeclaration(std::ostream& ofs, const Klass& onClass) const;

    void generateDefinition(std::ostream& ofs, const Klass& onClass) const;

private:
    int range2CvtArrayIndex(const Klass& onClass) const;

    void generateGetterForOneMandatory(std::ostream& ofs, const Klass& onClass) const;

    void generateGetterForOneOptional(std::ostream& ofs, const Klass& onClass) const;

    void generateGetterForMany(std::ostream& ofs, const Klass& onClass) const;

    void generateSetterForOne(std::ostream& ofs, const Klass& onClass) const;

    void generateSetterForMany(std::ostream& ofs, const Klass& onClass) const;
};

}
}

#endif //AUTORDF_CODEGEN_DATAPROPERTY_H
