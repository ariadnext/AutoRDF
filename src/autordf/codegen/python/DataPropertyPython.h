#ifndef AUTORDF_CODEGEN_DATAPROPERTY_H
#define AUTORDF_CODEGEN_DATAPROPERTY_H

#include <map>
#include <memory>
#include <ostream>

#include <autordf/ontology/AnnotationProperty.h>
#include <autordf/ontology/DataProperty.h>
#include <autordf/cvt/RdfTypeEnum.h>
#include "RdfsEntityPython.h"

namespace autordf {

namespace codegen {

class KlassPython;

class DataPropertyPython : public RdfsEntityPython {
    const ontology::Property& _decorated;
public:
    DataPropertyPython(const ontology::DataProperty& decorated) : RdfsEntityPython(decorated), _decorated(decorated) {}
    DataPropertyPython(const ontology::AnnotationProperty& decorated) : RdfsEntityPython(decorated), _decorated(decorated) {}

    void generate(std::ostream& ofs, const KlassPython& onClass) const;

    void generateKey(std::ostream& ofs, const KlassPython& onClass) const;

private:
    void generateGetterForOneMandatory(std::ostream& ofs, const KlassPython& onClass) const;

    void generateGetterForOneOptional(std::ostream& ofs, const KlassPython& onClass) const;

    void generateGetterForMany(std::ostream& ofs, const KlassPython& onClass) const;

    void generateSetterForOne(std::ostream& ofs, const KlassPython& onClass) const;

    void generateSetterForMany(std::ostream& ofs, const KlassPython& onClass) const;

    void generateRemover(std::ostream& ofs, const KlassPython& onClass) const;

    std::string orderedBoolValue() const;
};

}
}

#endif //AUTORDF_CODEGEN_DATAPROPERTY_H
