#include "autordf/ontology/Klass.h"

namespace autordf {
namespace ontology {

std::map <std::string, std::shared_ptr<Klass>> Klass::uri2Ptr;

std::set<std::shared_ptr<const Klass> > Klass::getAllAncestors() const {
    std::set<std::shared_ptr<const Klass> > all;
    for ( auto ancestor = directAncestors.begin(); ancestor != directAncestors.end(); ++ancestor ) {
        all.insert(uri2Ptr[*ancestor]);
        for ( std::shared_ptr<const Klass> more : uri2Ptr[*ancestor]->getAllAncestors() ) {
            all.insert(more);
        }
    }
    return all;
}
}
}


