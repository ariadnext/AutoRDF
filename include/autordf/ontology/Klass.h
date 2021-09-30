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
     * Functor used to compare shared_ptr on Property
     */
    class PropertyComparison {
    public:
        PropertyComparison() = default;
        /**
         * Comparison operator between two shared_ptr on Property
         * @param prop1
         * @param prop2
         * @return
         */
        bool operator() (const std::shared_ptr<Property>& prop1, const std::shared_ptr<Property>& prop2) const {
            return prop1->rdfname() < prop2->rdfname();
        }
    };

    /**
     * Types holding class properties
     */
    typedef std::set <std::shared_ptr<AnnotationProperty>, PropertyComparison> AnnotationPropertySet;
    typedef std::set <std::shared_ptr<DataProperty>, PropertyComparison> DataPropertySet;
    typedef std::set <std::shared_ptr<ObjectProperty>, PropertyComparison> ObjectPropertySet;

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
    const AnnotationPropertySet& annotationProperties() const { return _annotationProperties ; }

    /**
     * List of data properties for current class
     */
    const DataPropertySet& dataProperties() const { return _dataProperties; }

    /**
     * List of object properties for current class
     */
    const ObjectPropertySet& objectProperties() const { return _objectProperties; }

    /**
     * List of annotation keys for current class
     */
    const AnnotationPropertySet& annotationKeys() const { return _annotationKeys; }

    /**
     * List of data keys for current class
     */
    const DataPropertySet& dataKeys() const { return _dataKeys; }

    /**
     * List of object keys for current class
     */
    const ObjectPropertySet& objectKeys() const { return _objectKeys; }

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
    AnnotationPropertySet _annotationProperties;
    DataPropertySet _dataProperties;
    ObjectPropertySet _objectProperties;
    AnnotationPropertySet _annotationKeys;
    DataPropertySet _dataKeys;
    ObjectPropertySet _objectKeys;

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
