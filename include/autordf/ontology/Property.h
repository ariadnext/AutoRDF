#ifndef AUTORDF_ONTOLOGY_PROPERTY_H
#define AUTORDF_ONTOLOGY_PROPERTY_H

#include <list>
#include <string>
#include <map>

#include "RdfsEntity.h"

namespace autordf {
namespace ontology {

class Klass;

/**
 * Common ancestor to both DataProperty and ObjectProperty
 */
class Property : public RdfsEntity {
public:
    /**
     * Constructor
     */
    Property(const Ontology* ontology) : RdfsEntity(ontology), _minCardinality(0), _maxCardinality(0xFFFFFFFF), _ordered(false) {}

    /**
     * Lists of classes this property applies to
     */
    const std::list<std::string>& domains() const { return _domains; }

    /**
     * Returns the Rdf type this property must have in objects of given class
     * @param kls the class this property will be instantiated in. If nullptr qualified cardinality restrictions
     * will be ignored
     */
    Uri range(const Klass* kls = nullptr) const;

    /**
     * Returns the minimum number of times this property must be found in objects of given class
     * @param kls the class this property will be instanciated in
     */
    unsigned int minCardinality(const Klass& kls) const;

    /**
     * Returns the maximum number of times this property must be found in objects of given class
     * @param kls the class this property will be instanciated in
     */
    unsigned int maxCardinality(const Klass& kls) const;

    /**
     * When stored, this property items order must be preserved
     */
    unsigned int ordered() const { return _ordered; }

private:
    /**
     * Lists of classes this property applies to
     */
    std::list<std::string> _domains;

    /**
     * Rdf types instances of this property must be
     */
    std::string _range;

    /**
     * Minimum number of times this property must be present when it is found on an object
     */
    unsigned int _minCardinality;

    /**
     * Maximum number of times this property must be present when it is found on an object
     */
    unsigned int _maxCardinality;

    /**
     * When stored, this property items order must be preserved
     */
    unsigned int _ordered;

    friend class Ontology;
};

}
}

#endif //AUTORDF_ONTOLOGY_PROPERTY_H
