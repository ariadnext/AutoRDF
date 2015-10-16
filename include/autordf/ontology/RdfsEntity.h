#ifndef AUTORDF_ONTOLOGY_RDFSENDITY_H
#define AUTORDF_ONTOLOGY_RDFSENDITY_H

#include <string>
#include <iosfwd>

namespace autordf {

class Model;

namespace ontology {

/**
 * Parent class for OWL Properties and Classes
 */
class RdfsEntity {
public:
    /**
     * IRI for entity
     */
    std::string rdfname() const { return _rdfname; }

    /**
     * Label
     */
    std::string label() const { return _label; }

    /**
     * Entity comment
     */
    std::string comment() const { return _comment; }

    /**
     * Display friendly name: takes last part of IRI and make it something short and understandable for
     * human begin. It is also a valid C++ symbol name
     */
    std::string prettyIRIName(bool upperCaseFirst = false) const;

    static void setModel(Model *m) {
        _m = m;
    }

    static const Model* model() { return _m; }

    bool operator<(const RdfsEntity& other) const { return _rdfname < other._rdfname; }

private:
    static Model *_m;
    // Object iri
    std::string _rdfname;
    // rdfs comment
    std::string _comment;
    // rdfs label
    std::string _label;

    friend class Ontology;
};
}
}
#endif //AUTORDF_ONTOLOGY_RDFSENDITY_H
