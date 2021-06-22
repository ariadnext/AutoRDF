#ifndef AUTORDF_ONTOLOGY_RDFSENDITY_H
#define AUTORDF_ONTOLOGY_RDFSENDITY_H

#include <string>
#include <iosfwd>

#include "autordf/Uri.h"
#include <autordf/ontology/autordf-ontology_export.h>

#include <autordf/PropertyValue.h>

namespace autordf {

class Model;

namespace ontology {

class Ontology;

/**
 * @brief Parent class for Properties and Classes
 *
 * Stores iri, label, comments, and other metadata
 */
class RdfsEntity {
public:
    /**
     * Constructor
     */
    RdfsEntity(const Ontology* ontology) : _ontology(ontology) {};

    /**
     * IRI for entity
     */
    Uri rdfname() const { return _rdfname; }

    /**
     * Label
     */
    std::vector<PropertyValue> label() const { return _label; }

    /**
     * Entity comment
     */
    std::vector<PropertyValue> comment() const { return _comment; }

    /**
     * Pointer to addidinal data, see http://www.w3.org/TR/rdf-schema
     */
    std::vector<PropertyValue> seeAlso() const { return _seeAlso; }

    /**
     * Resource defining this entity, see http://www.w3.org/TR/rdf-schema
     */
    std::vector<PropertyValue> isDefinedBy() const { return _isDefinedBy; }

    /**
     * Display friendly name: takes last part of IRI and make it something short and understandable for
     * human begin. It is also a valid C++ symbol name
     */
    AUTORDF_ONTOLOGY_EXPORT std::string prettyIRIName(bool upperCaseFirst = false) const;

    /**
     * Returns the ontology this Entity belongs to
     */
    const Ontology* ontology() const { return _ontology; };

    /**
     * Used to insert this objet in maps
     */
    bool operator<(const RdfsEntity& other) const { return _rdfname < other._rdfname; }

protected:
    /**
     * The ontology this object belongs to
     */
    const Ontology* _ontology;

private:
    static Model *_m;
    // Object iri
    Uri _rdfname;
    // rdfs comment
    std::vector<PropertyValue> _comment;
    // rdfs label
    std::vector<PropertyValue> _label;
    // rdfs seeAlso
    std::vector<PropertyValue> _seeAlso;
    // rdfs isDefinedBy
    std::vector<PropertyValue> _isDefinedBy;

    // Easy write access for attributes instead of compilacted constructor
    friend class Ontology;
};
}
}
#endif //AUTORDF_ONTOLOGY_RDFSENDITY_H
