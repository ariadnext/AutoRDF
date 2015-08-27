#ifndef AUTORDF_CODEGEN_DATAPROPERTY_H
#define AUTORDF_CODEGEN_DATAPROPERTY_H

#include <map>
#include <memory>
#include <ostream>

#include "Property.h"

namespace autordf {
namespace codegen {

class DataProperty : public Property {
public:
    // iri to Property map
    static std::map<std::string, std::shared_ptr<DataProperty> > uri2Ptr;

    void generateDeclaration(std::ostream& ofs, const Klass& onClass) const;

    void generateDefinition(std::ostream& ofs, const Klass& onClass);

private:
    std::string getEffectiveRange(const Klass& kls) const;

    int range2CvtArrayIndex(const Klass& onClass) const;

    void generateForOneMandatory(std::ostream& ofs, const Klass& onClass) const;

    void generateForOneOptional(std::ostream& ofs, const Klass& onClass) const;

    void generateForMany(std::ostream& ofs, const Klass& onClass) const;
};

}
}

#endif //AUTORDF_CODEGEN_DATAPROPERTY_H
