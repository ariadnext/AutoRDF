#ifndef AUTORDF_OBJECT_H
#define AUTORDF_OBJECT_H

#include <memory>
#include <list>
#include <ostream>

#include <autordf/PropertyValue.h>
// FIXME: maybe remove ?
#include <autordf/Resource.h>

namespace autordf {

class Factory;

/**
 * Common Ancestor to all generated classes
 */
class Object {
public:

    /**
     * All newly Objects will be created in this Model
     */
    static void setFactory(Factory *f);

    /**
     * Creates new object, to given iri. If iri empty,
     * creates an anonymous (aka blank) object
     */
    Object(const std::string& iri = "");

    /**
     * Gets given property as Object
     * Property should be set.
     * @throw if property not set
     */
    Object getObject(const std::string& propertyIRI) const;

    /**
     * Returns given property as Object.
     * @returns pointer if property exists, null otherwise
     * It is caller responsibility to free resulting object
     */
    std::shared_ptr<Object> getOptionalObject(const std::string& propertyIRI) const;

    /**
     * Returns the list of object
     */
    std::list<Object> getObjectList(const std::string& propertyIRI) const;

    /**
     * Sets object to given property
     */
    void setObject(const std::string& propertyIRI, const Object& obj);

    /**
     * Sets list of objects to given property
     */
    void setObjectList(const std::string& propertyIRI, const std::list<Object>& values);


    PropertyValue getPropertyValue(const std::string& propertyIRI) const;

    std::shared_ptr<PropertyValue> getOptionalPropertyValue(const std::string& propertyIRI) const;

    std::list<PropertyValue> getPropertyValueList(const std::string& propertyIRI) const;

    void setPropertyValue(const std::string& propertyIRI, const PropertyValue& val);

    void setPropertyValueList(const std::string& propertyIRI, const std::list<PropertyValue>& values);

    /**
     * Provides ultra-fast trans-typing to another Object descendant
     */
    template<typename T> T as() {
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

    /**
     * Returns all Objects matching type specified as IRI
     */
    static std::list<Object> findByType(const std::string& iri = "");

    std::ostream& printStream(std::ostream&, int recurse = 0, int indentLevel = 0) const;

private:
    Resource _r;
    static Factory *_factory;

    Object(Resource r);
};

std::ostream& operator<<(std::ostream& os, const Object&);
}

#endif //AUTORDF_OBJECT_H
