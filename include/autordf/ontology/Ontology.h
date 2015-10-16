#ifndef AUTORDF_ONTOLOGY_ONTOLOGY_H
#define AUTORDF_ONTOLOGY_ONTOLOGY_H

#include <string>

namespace autordf {

class Factory;
class Object;

namespace ontology {

class RdfsEntity;

class Klass;

class Property;

class Ontology {
public:
    static void populateSchemaClasses(Factory *f, bool verbose = false);

    static const std::string RDF_NS;
    static const std::string RDFS_NS;
    static const std::string OWL_NS;

private:
    static bool _verbose;

    static void extractRDFS(const Object& o, RdfsEntity *rdfs);

    static void extractClassCardinality(const Object& o, Klass *kls, const char *card, const char *minCard,
                                        const char *maxCard);

    static void extractClass(const Object& o, Klass *kls);

    static void extractProperty(const Object& o, Property *prop);

    static void extractClass(const Object& rdfsClass);

    static void extractClasses(const std::string& classTypeIRI);
};
}
}

#endif //AUTORDF_ONTOLOGY_ONTOLOGY_H
