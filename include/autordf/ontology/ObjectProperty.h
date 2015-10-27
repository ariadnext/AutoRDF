#ifndef AUTORDF_ONTOLOGY_OBJECTPROPERTY_H
#define AUTORDF_ONTOLOGY_OBJECTPROPERTY_H

#include <memory>
#include <string>
#include <map>
#include <iosfwd>

#include "Property.h"

namespace autordf {
namespace ontology {

class ObjectProperty : public Property {
public:
    /**
     * Adds or overwrites the object in static map
     */
    static void add(const std::shared_ptr<ObjectProperty>& obj) { _uri2Ptr[obj->rdfname()] = obj; }

    /**
     * Finds using IRI
     * @throw std::out_of_range if not found
     */
    static const ObjectProperty& find(const std::string& iri) { return *_uri2Ptr.at(iri); }

    /**
     * Does the static map contains the given element ?
     */
    static bool contains(const std::string& iri) { return _uri2Ptr.count(iri); }

    /**
     * Maps IRI to object
     */
    static const std::map<std::string, std::shared_ptr<ObjectProperty> >& uri2Ptr() { return _uri2Ptr; }

    /**
     * Returns class for this property when found in kls object, or nullptr if no class is registered
     * @param kls class this objectproperty is suppoed to be instanciated in.
     * If nullptr, qualified cardinality restrictions will be ignored
     */
    std::shared_ptr<Klass> findClass(const Klass* kls = nullptr) const;

private:
    // iri to Property map
    static std::map<std::string, std::shared_ptr<ObjectProperty> > _uri2Ptr;
};
}
}
#endif //AUTORDF_ONTOLOGY_OBJECTPROPERTY_H
