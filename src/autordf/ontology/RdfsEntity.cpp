#include "autordf/ontology/RdfsEntity.h"

namespace autordf {
namespace ontology {

Model *RdfsEntity::_m = 0;

std::string RdfsEntity::prettyIRIName(bool uppercaseFirst) const {
    std::string cppname = _rdfname.prettyName(uppercaseFirst);
    if ( !::isalpha(cppname[0]) ) {
        cppname = "_" + cppname;
    }
    return cppname;
}

}
}

