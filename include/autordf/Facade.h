#ifndef AUTORDF_FACADE_H
#define AUTORDF_FACADE_H

namespace autordf {

class Facade {
public:
    ~Facade() { sync(); }

    /**
     * As the name suggests, removes all data
     * Associated with this object.
     * This only deletes properties and links to other objects,
     * It does not delete the objects we point to
     */
    void removePermanently();

    Resource resource();
    void assign(const Resource& resource);

    /**
     * Copies this object, with anonymous name (bnode)
     */
    Facade *clone();

    /**
     * Copies this object, to given iri
     */
    Facade *clone(const std::string& iri);

private:
    Resource _r;
};

}

#endif //AUTORDF_FACADE_H
