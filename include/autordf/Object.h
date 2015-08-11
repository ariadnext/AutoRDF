#ifndef AUTORDF_OBJECT_H
#define AUTORDF_OBJECT_H

#include <memory>
#include <list>
#include <ostream>
#include <stdexcept>

#include <autordf/Factory.h>
#include <autordf/PropertyValue.h>
#include <autordf/Resource.h>
#include <autordf/ResourceList.h>

namespace autordf {

class Factory;

/**
 * Common Ancestor to all generated classes
 * When copied, Objects are implicitly shared, with backend storage
 * provided by librdf.
 * New objects can be created in two ways:
 *  - Using Object(const std::string& iri) constructor
 *  - Using the clone() method
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
     * Build us using the same underlying resource as the other object
     */
    Object(const Object& obj);

    /**
     * Return object iri, or empty if it is a blank node
     */
    const std::string& iri() const;

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
    std::list<std::shared_ptr<Object> > getObjectList(const std::string& propertyIRI) const;

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
    template<typename T> std::shared_ptr<T> as() {
        return  std::shared_ptr<T>(new T(*this));
    }

    template<typename T> const std::shared_ptr<T> as() const {
        return  std::shared_ptr<T>(new T(*this));
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
    static std::list<Object> findByType(const std::string& typeIRI = "");

    /**
     * Dumps objects content to stream
     * @param recurse: if > 0, go down resource hierarchy by recurse level
     * @param indentLevel: How much layer of tabs to insert
     */
    std::ostream& printStream(std::ostream&, int recurse = 0, int indentLevel = 0) const;

protected:
    /**
     * Offered to descendants
     */
    template<typename T> static std::list<T> findHelper(const std::string& iri) {
        const ResourceList& rl = _factory->findByType(iri);
        std::list<T> objList;
        for(const Resource& r : rl) {
            objList.push_back(T(r));
        }
        return objList;
    }

    /**
     * Offered to descendants
     */
    template<typename T> std::list<T> getObjectListImpl(const std::string &propertyIRI) const {
        if ( propertyIRI.empty() ) {
            throw std::runtime_error("Calling getObjectListImpl() with empty IRI is forbidden");
        }
        std::list<T> objList;
        const std::list<Property>& propList = _r.getPropertyValues(propertyIRI);
        for (const Property& prop: propList) {
            objList.push_back(T(new typename T::element_type(prop.asResource())));
        }
        return objList;
    }

private:
    Resource _r;
    static Factory *_factory;

    Object(Resource r);
};

std::ostream& operator<<(std::ostream& os, const Object&);
}

#endif //AUTORDF_OBJECT_H
