#ifndef AUTORDF_OBJECT_H
#define AUTORDF_OBJECT_H

#include <memory>
#include <vector>
#include <iosfwd>
#include <stdexcept>
#include <map>
#include <set>

#include <autordf/Factory.h>
#include <autordf/PropertyValue.h>
#include <autordf/PropertyValueVector.h>
#include <autordf/Resource.h>
#include <autordf/Exception.h>
#include <autordf/Uri.h>

namespace autordf {

class Factory;

/**
 * Common Ancestor to all AutoRDF generated classes
 * When copied, Objects are implicitly shared, with backend storage
 * provided by librdf.
 * New objects can be created in two ways:
 *  - Using Object(const std::string& iri, const std::string& rdfTypeIRI = "") constructor: Creates a new object in Model
 *  - Using the clone(const std::string& iri) method: Makes a copy of object and write it to Model
 */
class Object {
public:
    /**
     * Full namespace for RDF, including #
     */
    static const std::string RDF_NS;

    /**
     * All newly Objects will be created in this Model
     */
    static void setFactory(Factory *f);

    /**
     * Creates new object, to given iri.
     * @param iri object IRI. If empty, creates an anonymous (aka blank) object
     * @param rdfTypeIRI If not empty, will write rdf type property when object is written
     * @param rtti Rdf Runtime type info, usually auto-generated. If the underlying resource has a type, makes sure it is compatible with
     *   rdfTypeIRI, using rtti type system
     * @throw InvalidClass if the Object is not of type rdfTypeIRI or one of its subclasses may be thrown only if both rdfTypeIRI and rtti are
     * not empty
     */
    Object(const Uri& iri = "", const Uri& rdfTypeIRI = "", const std::map<std::string, std::set<std::string> >* rtti = nullptr);

    /**
     * Build us using the same underlying resource as the other object
     *
     * This constructs a new C++ object that points to the same RDF data
     *
     * @param obj the object to use
     * @param rdfTypeIRI If not empty, will write rdf type property when object is written
     * @param rtti Rdf Runtime type info, usually auto-generated. If the underlying resource has a type, makes sure it is compatible with
     *   rdfTypeIRI, using rtti type system
     * @throw InvalidClass if the Object is not of type rdfTypeIRI or one of its subclasses may be thrown only if both rdfTypeIRI and rtti are
     * not empty
     */
    Object(const Object& obj, const Uri& rdfTypeIRI, const std::map<std::string, std::set<std::string> >* rtti = nullptr);

    /**
     * Optimized Copy constructor. Doest not perform any kind of type checking
     */
    Object(const Object& obj);

    /**
     * Return object iri, or empty if it is a blank node
     */
    Uri iri() const;

    /**
     * Return object iri, or empty if it is a blank node
     * If a prefix is defined for this iri in the underlying model, return a compressed IRI
     * e.g. http://my/horribly/long/iri#node --> my:node
     *
     * BEWARE: prefixed IRIs should only be used for display purpose, all AutoRDF internals
     * rely on full IRIs.
     */
    std::string prefixedIri() const;

    /**
     * Return types of current object.
     * If namespaceFilter is set, only types in this namespace will be returned
     * @param namespaceFilter A simple namespace
     */
    std::vector<Uri> getTypes(const std::string& namespaceFilter = "") const;

    /**
     * Gets given property as Object
     * Property should be set.
     * If property is instanciated more than one, return one of the values, with no particular rule
     * @throw PropertyNotFound if property not set
     */
    Object getObject(const Uri& propertyIRI) const;

    /**
     * Returns given property as Object.
     * If property is instanciated more than one, return one of the values, with no particular rule
     * @returns pointer if property exists, null otherwise
     */
    std::shared_ptr<Object> getOptionalObject(const Uri& propertyIRI) const;

    /**
     * Returns the list of object. If no object found returns empty vector
     */
    std::vector<Object> getObjectList(const Uri& propertyIRI) const;

    /**
     * Sets object to given property replacing existing value
     */
    void setObject(const Uri& propertyIRI, const Object& obj);

    /**
     * Adds object to given property
     * @param propertyIRI Internationalized Resource Identifiers property to set
     * @param obj object to add to the propertyIRI property
     */
    void addObject(const Uri& propertyIRI, const Object& obj);

    /**
     * Sets list of objects to given property
     */
    void setObjectList(const Uri& propertyIRI, const std::vector<Object>& values);

    /**
     * Remove the first value matching val for this object property
     * @param propertyIRI Internationalized Resource Identifiers property to remove one value from
     * @param obj object to remove from property value
     * @throw PropertyNotFound if propertyIRI has not obj as value
     */
    void removeObject(const Uri& propertyIRI, const Object& obj);

    /**
     * Gets given property value
     * Property should be set.
     * If property is instanciated more than one, return one of the values, with no particular rule
     * @throw PropertyNotFound if property not set
     */
    PropertyValue getPropertyValue(const Uri& propertyIRI) const;

    /**
     * Returns given property as Object.
     * If property is instanciated more than one, return one of the values, with no particular rule
     * @param propertyIRI Internationalized Resource Identifiers property to query
     * @returns pointer if property exists, null otherwise
     */
    std::shared_ptr<PropertyValue> getOptionalPropertyValue(const Uri& propertyIRI) const;

    /**
     * Returns the list of the values. If no value are found returns empty list
     * @param propertyIRI Internationalized Resource Identifiers property to query
     */
    PropertyValueVector getPropertyValueList(const Uri& propertyIRI) const;

    /**
     * Erases all previous values for property, and write unique value on place
     * @param propertyIRI Internationalized Resource Identifiers property to set
     * @param val unique value for property
     */
    void setPropertyValue(const Uri& propertyIRI, const PropertyValue& val);

    /**
     * Erases all previous values for property, and write value list on place
     * @param propertyIRI Internationalized Resource Identifiers property to set
     * @param values the list of values. All previous values are removed, and replaced with the given lists
     */
    void setPropertyValueList(const Uri& propertyIRI, const PropertyValueVector& values);

    /**
     * Adds value to this property, preserving all previous values;
     * @param propertyIRI Internationalized Resource Identifiers property to set
     * @param val value for property. This new value is added to the list of values for the Property
     */
    void addPropertyValue(const Uri& propertyIRI, const PropertyValue& val);

    /**
     * Remove the first value matching val for this property
     * @param propertyIRI Internationalized Resource Identifiers property to remove one value from
     * @param val value to remove
     * @throw PropertyNotFound if propertyIRI has not val as value
     */
    void removePropertyValue(const Uri& propertyIRI, const PropertyValue& val);

    /**
     * Forces the underlying resource to the type associated with this object.
     * This method can be called more than once, it effectively writes the type to the triple store only the first time it
     * is called.
     * This method is called by all setters of Object class
     */
    void writeRdfType();

    /**
     * Returns true if this object is also of the specified type IRI.
     * @return true If object type attribute contains typeIRI
     * @return false If type attribute does not contains typeIRI
     */
    bool isA(const Uri& typeIRI) const;

    /**
     * Returns true if this object is also of the type of the template argument object.
     * Template argument must an interface object generated by AutoRDF
     *
     * @return true If object type attribute contains the argument type
     * @return false If type attribute does not contains the argument type
     */
    template<typename T> bool isA() const {
        return isA(Uri(T::TYPEIRI));
    }

    /**
     * Provides ultra-fast trans-typing to another Object descendant
     * This is syntaxic sugar to say myobj.as<T>() instead of T(myobj)
     * @return transtyped object
     */
    template<typename T> T as() {
        return T(*this);
    }

    /**
     * Provides ultra-fast trans-typing to another Object descendant
     * This is syntaxic sugar to say myobj.as<T>() instead of T(myobj)
     * @return transtyped object
     */
    template<typename T> const T as() const {
        return T(*this);
    }

    /**
     * As the name suggests, removes all data
     * Associated with this object.
     * This only deletes properties and links to other objects,
     * It does not delete the objects we point to
     */
    void remove();

    /**
     * Copies this object, to given iri.
     *
     * Object is copied by duplicating all it properties values.
     * @param iri if empty, creates an anonymous (aka blank) object.
     */
    Object clone(const Uri& iri = "") const;

    /**
     * Returns all Objects matching type specified as IRI
     *
     * @param typeIRI Internationalized Resource Identifiers of the type to be retrieved
     */
    static std::vector<Object> findByType(const Uri& typeIRI = "");

    /**
     * Returns the only Object with property key set to value
     *
     * @param propertyIRI Internationalized Resource Identifier for the property
     * @param value value to look for
     *
     * @throw DuplicateObject if more than one object have the same property value
     * @throw ObjectNotFound if no object has given property with value
     */
    static Object findByKey(const Uri& propertyIRI, const PropertyValue& value);

    /**
     * Returns the only Object with property key set to value
     *
     * @param propertyIRI Internationalized Resource Identifier for the property
     * @param object resource to look for
     *
     * @throw DuplicateObject if more than one object have the same property value
     * @throw ObjectNotFound if no object has given property with value
     */
    static Object findByKey(const Uri& propertyIRI, const Object& object);

    /**
     * Dumps objects content to stream
     * @param recurse: if > 0, go down resource hierarchy by recurse level
     * @param indentLevel: How much layer of tabs to insert
     */
    std::ostream& printStream(std::ostream&, int recurse = 0, int indentLevel = 0) const;

    /**
     * Comparison operator used to store objects in maps for instance
     */
    bool operator<(const Object& obj) const {
        return _r.name() < obj._r.name();
    }

    /**
     * Equality operator
     */
    bool operator==(const Object& obj) const {
        return _r.name() == obj._r.name();
    }

    /**
     * Inverse Equality operator
     */
    bool operator!=(const Object& obj) const {
        return _r.name() != obj._r.name();
    }

    /**
     * Offered to interfaces
     */
    template<typename T> static std::vector<T> findHelper(const Uri& iri) {
        Statement query;
        query.predicate.setIri(RDF_NS + "type");
        query.object.setIri(iri);
        const StatementList& statements = factory()->find(query);
        std::vector<T> objList;
        for(const Statement& stmt : statements) {
            objList.push_back(T(factory()->createResourceFromNode(stmt.subject)));
        }
        return objList;
    }

    /**
     * Offered to interfaces
     * @throw InvalidIRI if propertyIRI is empty
     */
    template<typename T> std::vector<T> getObjectListImpl(const Uri& propertyIRI) const {
        if ( propertyIRI.empty() ) {
            throw InvalidIRI("Calling getObjectListImpl() with empty IRI is forbidden");
        }
        const std::list<Property>& propList = _r.getPropertyValues(propertyIRI);
        std::vector<T> objList;
        objList.reserve(propList.size());
        for (const Property& prop: propList) {
            if (prop.isResource()) {
                objList.push_back(T(prop.asResource()));
            }
        }
        return objList;
    }

    /**
     * Offered to interfaces
     * @throw InvalidIRI if propertyIRI is empty
     */
    template<typename T> void setObjectListImpl(const Uri& propertyIRI, const std::vector<T>& values) {
        writeRdfType();
        std::shared_ptr<Property> p =factory()->createProperty(propertyIRI);
        _r.removeProperties(propertyIRI);
        for (const Object& object : values) {
            p->setValue(object._r);
            _r.addProperty(*p);
        }
    }

    /**
     * Offered to interfaces
     * @throw InvalidIRI if propertyIRI is empty
     */
    template<cvt::RdfTypeEnum rdftype, typename T> std::vector<T> getValueListImpl(const Uri& propertyIRI) const {
        if ( propertyIRI.empty() ) {
            throw InvalidIRI("Calling getValueListImpl() with empty IRI is forbidden");
        }
        const std::list<Property>& propList = _r.getPropertyValues(propertyIRI);
        std::vector<T> valueList;
        valueList.reserve(propList.size());
        for (const Property& prop: propList) {
            valueList.push_back(prop.value().get<rdftype, T>());
        }
        return valueList;
    }

    /**
     * Offered to interfaces
     * @throw InvalidIRI if propertyIRI is empty
     */
    template<cvt::RdfTypeEnum rdftype, typename T> void setValueListImpl(const Uri& propertyIRI, const std::vector<T>& values) {
        writeRdfType();
        std::shared_ptr<Property> p = factory()->createProperty(propertyIRI);
        _r.removeProperties(propertyIRI);
        for (auto const & val: values) {
            p->setValue(PropertyValue().set<rdftype>(val));
            _r.addProperty(*p);
        }
    }

private:
    /**
     * The resource this object is based on
     */
    Resource _r;

    /**
     * The factory this object belongs to
     */
    static Factory *_factory;

    /**
     * True if rdf type value has already be written
     */
    bool _rdfTypeWritingRequired;
    /**
     * Lazy Type contains the (optional) rdf type for the object
     * Type writing is delayed until first object property is written in database
     */
    std::string _rdfTypeIRI;

    Object(Resource r, const Uri& rdfTypeIRI = "");

    /**
     * Shared constructor
     */
    void construct(const Uri& rdfTypeIRI);

    /**
     * Checks if current object is of rdf type that is either _rdfTypeIRI, or one of its subclasses
     * @param rdfTypesInfo, the types inferred for current class hierarchy
     */
    void runtimeTypeCheck(const std::map<std::string, std::set<std::string> > *rdfTypesInfo) const;

    /**
     * Returns the associated factory, or throws if nullptr
     */
    static Factory *factory();
};

/**
 * Writes object in json format.
 *
 * Is equivalent to printStream(os, 0, 0)
 */
std::ostream& operator<<(std::ostream& os, const Object&);
}

#endif //AUTORDF_OBJECT_H
