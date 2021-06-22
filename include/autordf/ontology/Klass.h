#ifndef AUTORDF_ONTOLOGY_KLASS_H
#define AUTORDF_ONTOLOGY_KLASS_H

#include <set>
#include <map>
#include <memory>
#include <string>

#include "RdfsEntity.h"
#include "AnnotationProperty.h"
#include "DataProperty.h"
#include "ObjectProperty.h"

#include <autordf/ontology/autordf-ontology_export.h>

namespace autordf {
namespace ontology {

/**
 * Models a class as seen by Semantic Web
 */
class Klass : public RdfsEntity {
public:
    using RdfsEntity::RdfsEntity;
    /**
     * Returns direct ancestors of this class
     */
    AUTORDF_ONTOLOGY_EXPORT std::set <std::shared_ptr<const Klass> > ancestors() const;

    /**
     * Returns direct predecessors of this class
     */
    AUTORDF_ONTOLOGY_EXPORT std::set <std::shared_ptr<Klass> > predecessors();

    /**
     * Returns direct predecessors of this class
     */
    AUTORDF_ONTOLOGY_EXPORT std::set <std::shared_ptr<const Klass> > predecessors() const;

    /**
     * Returns direct ancestors of this class
     */
    AUTORDF_ONTOLOGY_EXPORT std::set <std::shared_ptr<Klass> > ancestors();

    /**
     * If this class is defined using OWL oneOf construct, returns the list of Entities
     * this class is composed of
     */
    const std::set <RdfsEntity>& oneOfValues() const { return _oneOfValues; }

    /**
     * List of annotation properties for current class
     */
    const std::set <std::shared_ptr<AnnotationProperty>>& annotationProperties() const { return _annotationProperties ; }

    /**
     * List of data properties for current class
     */
    const std::set <std::shared_ptr<DataProperty>>& dataProperties() const { return _dataProperties; }

    /**
     * List of object properties for current class
     */
    const std::set <std::shared_ptr<ObjectProperty>>& objectProperties() const { return _objectProperties; }

    /**
     * List of annotation keys for current class
     */
    const std::set <std::shared_ptr<AnnotationProperty>>& annotationKeys() const { return _annotationKeys; }

    /**
     * List of data keys for current class
     */
    const std::set <std::shared_ptr<DataProperty>>& dataKeys() const { return _dataKeys; }

    /**
     * List of object keys for current class
     */
    const std::set <std::shared_ptr<ObjectProperty>>& objectKeys() const { return _objectKeys; }

    /**
     * Returns all (direct and indirect) ancestors for this class
     */
    AUTORDF_ONTOLOGY_EXPORT std::set <std::shared_ptr<const Klass>> getAllAncestors() const;

    /**
     * Returns all (direct and indirect) predecessors for this class
     */
    AUTORDF_ONTOLOGY_EXPORT std::set<std::shared_ptr<const Klass> > getAllPredecessors() const;

    /*
     * ===================================================================
     * All methods below are purely internal
     * ===================================================================
     */

    /**
     * Take into account qualified cardinality restrictions, that allow to owerride the
     * the data type of this property for a specific class of object
     */
    const std::map <std::string, std::string>& overridenRange() const { return _overridenRange; }

    /**
     * Take into account cardinality restrictions, that allow to owerride the cardinality for this class instance
     */
    const std::map<std::string, unsigned int>& overridenMinCardinality() const { return _overridenMinCardinality; }

    /**
     * Take into account cardinality restrictions, that allow to owerride the cardinality for this class instance
     */
    const std::map<std::string, unsigned int>& overridenMaxCardinality() const { return _overridenMaxCardinality; }

private:
    std::set <std::string> _directAncestors;
    std::set <std::shared_ptr<AnnotationProperty>> _annotationProperties;
    std::set <std::shared_ptr<DataProperty>> _dataProperties;
    std::set <std::shared_ptr<ObjectProperty>> _objectProperties;
    std::set <std::shared_ptr<AnnotationProperty>> _annotationKeys;
    std::set <std::shared_ptr<DataProperty>> _dataKeys;
    std::set <std::shared_ptr<ObjectProperty>> _objectKeys;

    /**
     * Iri to range map
     */
    std::map <std::string, std::string> _overridenRange;
    // It is usual (but optional) to specify at class level the minimum and/or maximum instances for a property
    // This is done using cardinality restrictions
    std::map<std::string, unsigned int> _overridenMinCardinality;
    std::map<std::string, unsigned int> _overridenMaxCardinality;
    /** owl:oneOf Classes defined with the oneOf construct have this set set */
    std::set <RdfsEntity> _oneOfValues;
    // owl:hasKey
    std::set<std::string> _keys;

    friend class Ontology;
};

}
}
#endif //AUTORDF_ONTOLOGY_KLASS_H
