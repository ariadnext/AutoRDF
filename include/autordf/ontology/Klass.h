#ifndef AUTORDF_ONTOLOGY_KLASS_H
#define AUTORDF_ONTOLOGY_KLASS_H

#include <set>
#include <map>
#include <memory>
#include <string>

#include "RdfsEntity.h"
#include "DataProperty.h"
#include "ObjectProperty.h"

namespace autordf {
namespace ontology {

class Klass : public RdfsEntity {
public:
    using RdfsEntity::RdfsEntity;
    /**
     * Returns direct ancestors of this class
     */
    std::set <std::shared_ptr<const Klass> > ancestors() const;

    /**
     * Returns direct ancestors of this class
     */
    std::set <std::shared_ptr<Klass> > ancestors();

    /**
     * If this class is defined using OWL oneOf construct, returns the list of Entities
     * this class is composed of
     */
    const std::set <RdfsEntity>& oneOfValues() const { return _oneOfValues; }

    /**
     * List of data properties for current class
     */
    const std::set <std::shared_ptr<DataProperty>>& dataProperties() const { return _dataProperties; }

    /**
     * List of object properties for current class
     */
    const std::set <std::shared_ptr<ObjectProperty>>& objectProperties() const { return _objectProperties; }

    /**
     * Returns all (direct and indirect) ancestors for this class
     */
    std::set <std::shared_ptr<const Klass>> getAllAncestors() const;

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
    std::set <std::shared_ptr<DataProperty>> _dataProperties;
    std::set <std::shared_ptr<ObjectProperty>> _objectProperties;
    /**
     * Iri to range map
     */
    std::map <std::string, std::string> _overridenRange;
    // It is usual (but optional) to specify at class level the minimum and/or maximum instances for a property
    // This is done using cardinality restrictions
    std::map<std::string, unsigned int> _overridenMinCardinality;
    std::map<std::string, unsigned int> _overridenMaxCardinality;
    /** Classes defined with the oneOf construct have this set set */
    std::set <RdfsEntity> _oneOfValues;

    friend class Ontology;
};

}
}
#endif //AUTORDF_ONTOLOGY_KLASS_H
