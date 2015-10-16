#include "autordf/ontology/RdfsEntity.h"

namespace autordf {
namespace ontology {

Model *RdfsEntity::_m = 0;

std::string RdfsEntity::prettyIRIName(bool uppercaseFirst) const {
    std::string cppname = _rdfname.substr(_rdfname.find_last_of("/#:") + 1);
    if ( !::isalpha(cppname[0]) ) {
        cppname = "_" + cppname;
    }
    if ( uppercaseFirst ) {
        cppname[0] = ::toupper(cppname[0]);
    }
    return cppname;
}

}
}

