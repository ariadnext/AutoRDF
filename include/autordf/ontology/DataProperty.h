#ifndef AUTORDF_ONTOLOGY_DATAPROPERTY_H
#define AUTORDF_ONTOLOGY_DATAPROPERTY_H

#include <map>
#include <memory>

#include "Property.h"

namespace autordf {
namespace ontology {

class DataProperty : public Property {
public:
    /**
     * Adds or overwrites the object in static map
     */
    static void add(const std::shared_ptr<DataProperty>& obj) { _uri2Ptr[obj->rdfname()] = obj; }

    /**
     * Finds using IRI
     * @throw std::out_of_range if not found
     */
    static const DataProperty& find(const std::string& iri) { return *_uri2Ptr.at(iri); }

    /**
     * Does the static map contains the given element ?
     */
    static bool contains(const std::string& iri) { return _uri2Ptr.count(iri); }

    /**
     * Maps IRI to object
     */
    static const std::map<std::string, std::shared_ptr<DataProperty> >& uri2Ptr() { return _uri2Ptr; }

private:
    // iri to Property map
    static std::map<std::string, std::shared_ptr<DataProperty> > _uri2Ptr;
};

}
}

#endif //AUTORDF_ONTOLOGY_DATAPROPERTY_H
