#include "autordf/ontology/Klass.h"

namespace autordf {
namespace ontology {

std::map <std::string, std::shared_ptr<Klass>> Klass::uri2Ptr;

std::set <std::shared_ptr<const Klass> > Klass::ancestors() const {
    std::set<std::shared_ptr<const Klass> > s;
    for (auto ancestor = _directAncestors.begin(); ancestor != _directAncestors.end(); ++ancestor) {
        s.insert(uri2Ptr[*ancestor]);
    }
    return s;
}

std::set <std::shared_ptr<Klass> > Klass::ancestors() {
    std::set<std::shared_ptr<Klass> > s;
    for (auto ancestor = _directAncestors.begin(); ancestor != _directAncestors.end(); ++ancestor) {
        s.insert(uri2Ptr[*ancestor]);
    }
    return s;
}

std::set<std::shared_ptr<const Klass> > Klass::getAllAncestors() const {
    std::set<std::shared_ptr<const Klass> > all;
    for ( auto ancestor = _directAncestors.begin(); ancestor != _directAncestors.end(); ++ancestor ) {
        all.insert(uri2Ptr[*ancestor]);
        for ( std::shared_ptr<const Klass> more : uri2Ptr[*ancestor]->getAllAncestors() ) {
            all.insert(more);
        }
    }
    return all;
}
}
}


