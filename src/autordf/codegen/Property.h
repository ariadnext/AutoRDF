#ifndef AUTORDF_CODEGEN_PROPERTY_H
#define AUTORDF_CODEGEN_PROPERTY_H

#include <list>
#include <string>

#include "RdfsEndity.h"

namespace autordf {
namespace codegen {

class Klass;

class Property : public RdfsEntity {
public:
    std::list<std::string> domains;
    std::string range;
    unsigned int minCardinality;
    unsigned int maxCardinality;

    //FIXME: this behaviour should be checked against the standard
    unsigned int getEffectiveMinCardinality(const Klass& kls) const;

    //FIXME: this behaviour should be checked against the standard
    unsigned int getEffectiveMaxCardinality(const Klass& kls) const;
};

}
}

#endif //AUTORDF_CODEGEN_PROPERTY_H
