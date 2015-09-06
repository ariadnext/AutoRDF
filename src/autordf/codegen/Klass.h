#ifndef AUTORDF_CODEGEN_KLASS_H
#define AUTORDF_CODEGEN_KLASS_H

#include <set>
#include <map>
#include <memory>
#include <string>

#include "RdfsEndity.h"
#include "DataProperty.h"
#include "ObjectProperty.h"

namespace autordf {
namespace codegen {

class Klass : public RdfsEntity {
public:
    std::set <std::string> directAncestors;
    std::set <RdfsEntity> enumValues;

    std::set <std::shared_ptr<DataProperty>> dataProperties;
    std::set <std::shared_ptr<ObjectProperty>> objectProperties;

    // It is usual (but optional) to specify at class level the minimum and/or maximum instances for a property
    // This is done using cardiniality restrictions
    std::map<std::string, unsigned int> overridenMinCardinality;
    std::map<std::string, unsigned int> overridenMaxCardinality;
    // Qualified Cardinality restrictions also allow to specify a range for this instance properties
    std::map <std::string, std::string> overridenRange;

    std::set <std::shared_ptr<const Klass>> getClassDependencies() const;

    std::set <std::shared_ptr<const Klass>> getAllAncestors() const;

    void generateInterfaceDeclaration() const;

    void generateInterfaceDefinition() const;

    void generateDeclaration() const;

    void generateDefinition() const;

    // iri to Klass map
    static std::map <std::string, std::shared_ptr<Klass>> uri2Ptr;

    static std::string outdir;
};

}
}
#endif //AUTORDF_CODEGEN_KLASS_H
