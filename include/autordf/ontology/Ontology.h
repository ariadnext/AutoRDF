#ifndef AUTORDF_ONTOLOGY_ONTOLOGY_H
#define AUTORDF_ONTOLOGY_ONTOLOGY_H

#include <string>
#include <map>
#include <memory>

#include "autordf/ontology/Klass.h"
#include "autordf/ontology/DataProperty.h"
#include "autordf/ontology/ObjectProperty.h"

namespace autordf {

class Factory;
class Model;
class Object;

namespace ontology {

class RdfsEntity;

class Klass;

class Property;

/**
 * Maps an OWL or RDFS Ontology to set of more readily accessible C++ objects
 */
class Ontology {
public:
    /**
     * Default constructor
     * @param f where to read this ontology from
     * @param verbose if true prints debug output to standard output
     */
    Ontology(Factory *f, bool verbose = false);

    /**
     * Finds class using IRI
     * @throw std::out_of_range if not found
     */
    std::shared_ptr<const Klass> findClass(const std::string& iri) const { return _classUri2Ptr.at(iri); }

    /**
     * Does the static map contains the given element class ?
     */
    bool containsClass(const std::string& iri) const { return _classUri2Ptr.count(iri); }

    /**
     * Maps IRI to object class
     */
    const std::map<std::string, std::shared_ptr<Klass>>& classUri2Ptr() const { return _classUri2Ptr; }

    /**
     * Finds object property using IRI
     * @throw std::out_of_range if not found
     */
    const ObjectProperty& findObjectProperty(const std::string& iri) const { return *_objectPropertyUri2Ptr.at(iri); }

    /**
     * Does the static map contains the given object property ?
     */
    bool containsObjectProperty(const std::string& iri) const { return _objectPropertyUri2Ptr.count(iri); }

    /**
     * Maps IRI to object property
     */
    const std::map<std::string, std::shared_ptr<ObjectProperty> >& objectPropertyUri2Ptr() const { return _objectPropertyUri2Ptr; }

    /**
     * Finds DataProperty using IRI
     * @throw std::out_of_range if not found
     */
    const DataProperty& findDataProperty(const std::string& iri) const { return *_dataPropertyUri2Ptr.at(iri); }

    /**
     * Does the static map contains the given DataProperty element ?
     */
    bool containsDataProperty(const std::string& iri) const { return _dataPropertyUri2Ptr.count(iri); }

    /**
     * Maps IRI to DataProperty
     */
    const std::map<std::string, std::shared_ptr<DataProperty> >& dataPropertyUri2Ptr() const { return _dataPropertyUri2Ptr; }

    /**
     * The model this ontology has been loaded from
     */
    const Model* model() const;

    /**
     * RDF namespace prefix
     */
    static const std::string RDF_NS;

    /**
     * RDFS namespace prefix
     */
    static const std::string RDFS_NS;

    /**
     * OWL namespace prefix
     */
    static const std::string OWL_NS;

private:
    /**
     * Reads ontology from given Factory
     */
    void populateSchemaClasses(Factory *f);

    /**
     * Adds or overwrites the object in static map
     */
    void addClass(const std::shared_ptr<Klass>& obj) { _classUri2Ptr[obj->rdfname()] = obj; }

    /**
     * Adds or overwrites the object in static map
     */
    void addObjectProperty(const std::shared_ptr<ObjectProperty>& obj) { _objectPropertyUri2Ptr[obj->rdfname()] = obj; }

    /**
     * Adds or overwrites the object in static map
     */
    void addDataProperty(const std::shared_ptr<DataProperty>& obj) { _dataPropertyUri2Ptr[obj->rdfname()] = obj; }

    /**
     * Display some internal details about code generation on std::cout
     */
    bool _verbose;

    Factory *_f;

    std::map<std::string, std::shared_ptr<Klass>> _classUri2Ptr;
    std::map<std::string, std::shared_ptr<ObjectProperty> > _objectPropertyUri2Ptr;
    std::map<std::string, std::shared_ptr<DataProperty> > _dataPropertyUri2Ptr;

    void extractRDFS(const Object& o, RdfsEntity *rdfs);

    void extractClassCardinality(const Object& o, Klass *kls, const char *card, const char *minCard,
                                        const char *maxCard);

    void extractClass(const Object& o, Klass *kls);

    void extractProperty(const Object& o, Property *prop);

    void extractClass(const Object& rdfsClass);

    void extractClasses(const std::string& classTypeIRI);

    void addHardcodedAnnotationProperty(const std::string& iri, const std::string& label);
};
}
}

#endif //AUTORDF_ONTOLOGY_ONTOLOGY_H
