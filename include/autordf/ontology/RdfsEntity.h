#ifndef AUTORDF_ONTOLOGY_RDFSENDITY_H
#define AUTORDF_ONTOLOGY_RDFSENDITY_H

#include <string>
#include <iosfwd>

namespace autordf {

class Model;

namespace ontology {

class RdfsEntity {
public:
    // Object iri
    std::string rdfname;

    // rdfs comment
    std::string comment;
    // rdfs label
    std::string label;

    static void setModel(Model *m) {
        _m = m;
    }

    static const Model* model() { return _m; }

    bool operator<(const RdfsEntity& other) const { return rdfname < other.rdfname; }

private:
    static Model *_m;
};
}
}
#endif //AUTORDF_ONTOLOGY_RDFSENDITY_H
