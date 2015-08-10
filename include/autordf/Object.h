#ifndef AUTORDF_OBJECT_H
#define AUTORDF_OBJECT_H

namespace autordf {

/**
 * Common Ancestor to all generated classes
 */
class Object {
public:
    /**
     * Copies this object, to given iri. If iri empty,
     * creates an anonymous (aka blank) object
     */
    Object(const std::string& iri = "");

    /**
     * All newly Objects will be created in this Model
     */
    static void setFactory(Factory *f);

    /**
     * Provides utra-fast transtyping to another Object descendant
     */
    template<typename T> T as<T>() {
        T object;
        object._r = _r;
        return object;
    }

    /**
     * As the name suggests, removes all data
     * Associated with this object.
     * This only deletes properties and links to other objects,
     * It does not delete the objects we point to
     */
    void remove();

    /**
     * Copies this object, to given iri. If iri empty,
     * creates an anonymous (aka blank) object
     */
    Object clone(const std::string& iri = "");

protected:
    Resource _r;
    static Factor *_factory;
};

}

#endif //AUTORDF_OBJECT_H
