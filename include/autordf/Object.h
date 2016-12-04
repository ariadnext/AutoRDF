#ifndef AUTORDF_OBJECT_H
#define AUTORDF_OBJECT_H

#include <memory>
#include <vector>
#include <stack>
#include <iosfwd>
#include <stdexcept>
#include <map>
#include <set>
#include <functional>

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
     * Well known RDF identifier
     */
    static const std::string RDF_TYPE;
    /**
     * Well known RDF identifier
     */
    static const std::string RDF_STATEMENT;
    /**
     * Well known RDF identifier
     */
    static const std::string RDF_SUBJECT;
    /**
     * Well known RDF identifier
     */
    static const std::string RDF_PREDICATE;
    /**
     * Well known RDF identifier
     */
    static const std::string RDF_OBJECT;

    /**
     * All newly Objects, and new values will be created in this Factory
     */
    static void setFactory(Factory *f);

    /**
     * All newly Objects, and new values will be created in this Factory.
     * Call popFactory() to restore previously set factory
     */
    static void pushFactory(Factory *f);

    /**
     * All newly Objects, and new values will be created in this Model
     */
    static void popFactory();

    /**
     * Creates new object, to given iri.
     * @param iri object IRI. If empty, creates an anonymous (aka blank) object
     * @param rdfTypeIRI If not empty, will write rdf type property when object is written
     * not empty
     */
    Object(const Uri& iri = "", const Uri& rdfTypeIRI = "");

    /**
     * Build us using the same underlying resource as the other object
     *
     * This constructs a new C++ object that points to the same RDF data
     *
     * @param obj the object to use
     */
    Object(const Object& obj);

    /**
     * Return object iri, or empty if it is a blank node
     */
    Uri iri() const;

    /**
     * Return object Qualified Name, or empty if it is a blank node
     * If a prefix is defined for this iri in the underlying model, return a Qualified Name
     * e.g. http://my/horribly/long/iri#node --> my:node
     *
     * BEWARE: prefixed IRIs should only be used for display purpose, all AutoRDF internals
     * rely on full IRIs.
     */
    std::string QName() const;

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
     * @param propertyIRI Internationalized Resource Identifiers property to get
     * @param preserveOrdering if true, order of values stored in RDF model will be used to restore original saving order
     * @throw CannotPreserveOrder if preserveOrdering is set and ordering data is not found in model
     */
    std::vector<Object> getObjectList(const Uri& propertyIRI, bool preserveOrdering = false) const;

    /**
     * Sets object to given property replacing existing value
     * @param propertyIRI Internationalized Resource Identifiers property to set value of
     */
    void setObject(const Uri& propertyIRI, const Object& obj);

    /**
     * Adds object to given property
     * @param propertyIRI Internationalized Resource Identifiers property to set
     * @param obj object to add to the propertyIRI property
     * @param preserveOrdering if true, order of values will be store in RDF model. This is a non standard AutoRDF extension.
     */
    void addObject(const Uri& propertyIRI, const Object& obj, bool preserveOrdering = false);

    /**
     * Sets list of objects to given property
     * @param propertyIRI Internationalized Resource Identifiers property to set value of
     * @param values list of values
     * @param preserveOrdering if true, order of values will be store in RDF model. This is a non standard AutoRDF extension.
     */
    void setObjectList(const Uri& propertyIRI, const std::vector<Object>& values, bool preserveOrdering = false);

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
     * @param propertyIRI Internationalized Resource Identifiers property to get
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
     * @param preserveOrdering if true, order of values stored in RDF model will be used to restore original saving order
     * @throw CannotPreserveOrder if preserveOrdering is set and ordering data is not found in model
     */
    PropertyValueVector getPropertyValueList(const Uri& propertyIRI, bool preserveOrdering = false) const;

    /**
     * Erases all previous values for property, and write unique value on place
     * @param propertyIRI Internationalized Resource Identifiers property to set
     * @param val unique value for property
     * @throw CannotUnreify if this property already has a reified a value with additional data
     */
    void setPropertyValue(const Uri& propertyIRI, const PropertyValue& val);

    /**
     * Erases all previous values for property, and write value list on place
     * @param propertyIRI Internationalized Resource Identifiers property to set
     * @param values the list of values. All previous values are removed, and replaced with the given lists
     * @param preserveOrdering if true, order of values will be store in RDF model. This is a non standard AutoRDF extension.
     */
    void setPropertyValueList(const Uri& propertyIRI, const PropertyValueVector& values, bool preserveOrdering = false);

    /**
     * Adds value to this property, preserving all previous values;
     * @param propertyIRI Internationalized Resource Identifiers property to set
     * @param val value for property. This new value is added to the list of values for the Property
     * @param preserveOrdering if true, order of values will be store in RDF model. This is a non standard AutoRDF extension.
     */
    void addPropertyValue(const Uri& propertyIRI, const PropertyValue& val, bool preserveOrdering = false);

    /**
     * Remove the first value matching val for this property
     * @param propertyIRI Internationalized Resource Identifiers property to remove one value from
     * @param val value to remove
     * @throw PropertyNotFound if propertyIRI has not val as value
     * @throw CannotUnreify if value is store as a reified statement and statement cannot be unreified
     */
    void removePropertyValue(const Uri& propertyIRI, const PropertyValue& val);

    /**
     * Writes a data property in reified form.
     *
     * If statement is already reified, returns already reified statement.
     * After reification succeeds, the non-reified RDF statement is removed
     *
     * RDF represents a reified statement as four statements with particular RDF properties and objects: the statement (S, P, O), reified by resource R, is represented by:
     *
     * R rdf:type rdf:Statement
     * R rdf:subject S
     * R rdf:predicate P
     * R rdf:object O
     *
     * @param propertyIRI Internationalized Resource Identifiers of data property
     * @param val value to reify
     * @return Object representing the reified statement
     */
    Object reifyPropertyValue(const Uri& propertyIRI, const PropertyValue& val);

    /**
     * Turns a reified statement into a simple statement.
     *
     * If statement is already in unreified form, does nothing
     * If reified statement contains more than rdf:type, rdf:subject, rdf:predicate and rdf:object, convertion fails,
     * and CannotUnreify exception is thrown
     *
     * @param propertyIRI Internationalized Resource Identifiers of data property
     * @param val value to unreify
     * @param keep if set to  false, statement is not only unreified, but also completely erased
     * @throw CannotUnreify is unreification is not possible
     * @return true if there was a statement to unReify, false otherwise
     */
    bool unReifyPropertyValue(const Uri& propertyIRI, const PropertyValue& val, bool keep = true);

    /**
     * Test if data property value is stored as a RDF reified form, or simple statement (default)
     * @param propertyIRI Internationalized Resource Identifiers of data property
     * @param val value to test
     * @return Object representing the reified statement if available, otherwise nullptr
     */
    std::shared_ptr<Object> reifiedPropertyValue(const Uri& propertyIRI, const PropertyValue& val) const;

    /**
     * Writes an object property in reified form.
     *
     * If statement is already reified, returns already reified statement.
     * After reification succeeds, the non-reified RDF statement is removed
     *
     * RDF represents a reified statement as four statements with particular RDF properties and objects: the statement (S, P, O), reified by resource R, is represented by:
     *
     * R rdf:type rdf:Statement
     * R rdf:subject S
     * R rdf:predicate P
     * R rdf:object O
     *
     * @param propertyIRI Internationalized Resource Identifiers of data property
     * @param object value to reify
     * @return Object representing the reified statement
     */
    Object reifyObject(const Uri& propertyIRI, const Object& object);

    /**
     * Turns a reified statement into a simple statement.
     *
     * If statement is already in unreified form, does nothing
     * If reified statement contains more than rdf:type, rdf:subject, rdf:predicate and rdf:object, convertion fails,
     * and CannotUnreify exception is thrown
     *
     * @param propertyIRI Internationalized Resource Identifiers of data property
     * @param object value to unreify
     * @param keep if set to  false, statement is not only unreified, but also completely erased
     * @throw CannotUnreify is unreification is not possible
     * @return true if there was a statement to unReify, false otherwise
     */
    bool unReifyObject(const Uri& propertyIRI, const Object& object, bool keep = true);

    /**
     * Test if object property value is stored as a RDF reified form, or simple statement (default)
     * @param propertyIRI Internationalized Resource Identifiers of data property
     * @param object value to test
     * @return Object representing the reified statement if available, otherwise nullptr
     */
    std::shared_ptr<Object> reifiedObject(const Uri& propertyIRI, const Object& object) const;

    /**
     * Forces the underlying resource to the type associated with this object.
     * This method can be called more than once, it effectively writes the type to the triple store only the first time it
     * is called.
     * This method is called by all setters of Object class
     * @return current object
     */
    Object& writeRdfType();

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
     * Returns all Objects in the factory
     */
    static std::set<Object> findAll();

    /**
     * Find all objects pointing to the current object
     * @return
     */
    std::set<Object> findSources() const;

    /**
     * Find all objects current object is pointing out
     * @return
     */
    std::set<Object> findTargets() const;

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
    template<typename T> std::vector<T> getObjectListImpl(const Uri& propertyIRI, bool preserveOrdering) const {
        if ( propertyIRI.empty() ) {
            throw InvalidIRI("Calling getObjectListImpl() with empty IRI is forbidden");
        }
        const std::shared_ptr<std::list<Property>>& propList = _r.getPropertyValues(propertyIRI);
        std::vector<T> objList;
        objList.reserve(propList->size());
        for (const Property& prop: *propList) {
            if (prop.isResource()) {
                objList.emplace_back(T(prop.asResource()));
            }
        }
        if ( !preserveOrdering ) {
            // Iterate through statements and find ones matching predicate
            reifiedObjectIterate(propertyIRI, [&objList](const Object& o) {
                objList.emplace_back(T(o));
            });
            return objList;
        } else {
            if ( objList.size() ) {
                throw CannotPreserveOrder("Unable to read back statements order as there is at least one statement without ordering info");
            } else {
                typedef std::pair<long long, Object> ObjectWithOrder;
                std::vector<ObjectWithOrder> unordered;
                reifiedObjectIterate(propertyIRI, [&](const Object& o) {
                    std::shared_ptr<Property> orderprop = reifiedObjectAsResource(propertyIRI, o)->getOptionalProperty(AUTORDF_ORDER);
                    if ( !orderprop ) {
                        throw CannotPreserveOrder("Unable to read back statements order as there is at least one statement without ordering info");
                    }
                    unordered.emplace_back(std::make_pair(orderprop->value().get<cvt::RdfTypeEnum::xsd_integer, long long>(), o));
                });
                std::sort(unordered.begin(), unordered.end(), [](const ObjectWithOrder& a, const ObjectWithOrder& b) {
                    return a.first < b.first;
                });
                for ( const ObjectWithOrder& pvo : unordered ) {
                    objList.emplace_back(pvo.second);
                }
                return objList;
            }
        }
    }

    /**
     * Offered to interfaces
     * @throw InvalidIRI if propertyIRI is empty
     */
    template<typename T> void setObjectListImpl(const Uri& propertyIRI, const std::vector<T>& values, bool preserveOrdering) {
        writeRdfType();
        removeAllReifiedObjectPropertyStatements(propertyIRI);
        std::shared_ptr<Property> p =factory()->createProperty(propertyIRI);
        _r.removeProperties(propertyIRI);

        if ( !preserveOrdering ) {
            for (const Object& object : values) {
                p->setValue(object._r);
                _r.addProperty(*p);
            }
        } else {
            long long i = 1;
            for (const Object& object : values) {
                Resource reified = createReificationResource(propertyIRI, object._r);
                std::shared_ptr<Property> order = factory()->createProperty(AUTORDF_ORDER);
                order->setValue(PropertyValue().set<cvt::RdfTypeEnum::xsd_integer>(i));
                reified.addProperty(*order);
                ++i;
            }
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
        const std::shared_ptr<std::list<Property>>& propList = _r.getPropertyValues(propertyIRI);
        std::vector<T> valueList;
        valueList.reserve(propList->size());
        for (const Property& prop: *propList) {
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
     * The factory this object belongs to. Factory used for this object is found at the top of the stack
     */
    static std::stack<Factory *> _factories;

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
     * Creates a new resource that stores a value for this resource
     */
    Resource createReificationResource(const Uri& propertyIRI, const PropertyValue& val);

    /**
     * Creates a new resource that stores a value for this resource
     */
    Resource createReificationResource(const Uri& propertyIRI, const Resource& val);

    /**
     * Return all reified values for given property
     */
    void reifiedPropertyValueIterate(const Uri& propertyIRI, std::function<void (const PropertyValue&)> cb) const;

    /**
     * Return all reified values for given property
     */
    void reifiedObjectIterate(const Uri& propertyIRI, std::function<void (const Object& obj)> cb) const;

    /**
     * Returns the first found reified PropertyValue, if found
     */
    std::shared_ptr<PropertyValue> reifiedPropertyValueOptional(const Uri& propertyIRI) const;

    /**
     * Returns the first found reified object, if found
     */
    std::shared_ptr<Object> reifiedObjectOptional(const Uri& propertyIRI) const;

    /**
     * Removes all reified statements
     * @throw CannotUnreify if one statement cannot be unreified
     */
    void removeAllReifiedObjectPropertyStatements(const Uri& propertyIRI);

    /**
     * Removes all reified statements
     * @throw CannotUnreify if one statement cannot be unreified
     */
    void removeAllReifiedDataPropertyStatements(const Uri& propertyIRI);

    /**
     * @internal
     */
    NodeList reificationResourcesForCurrentObject() const;

    /**
     * Test if val is stored as a RDF reified form, or simple statement (default)
     * @param propertyIRI Internationalized Resource Identifiers of data property
     * @param val value to test
     * @return Object representing the reified statement if available, otherwise nullptr
     */
    std::shared_ptr<Resource> reifiedPropertyValueAsResource(const Uri& propertyIRI, const PropertyValue& val) const;

    /**
     * Test if val is stored as a RDF reified form, or simple statement (default)
     * @param propertyIRI Internationalized Resource Identifiers of object property
     * @param val value to test
     * @return Object representing the reified statement if available, otherwise nullptr
     */
    std::shared_ptr<Resource> reifiedObjectAsResource(const Uri& propertyIRI, const Object& val) const;

    /**
     * Checks if it is possible to unreify statement, i.e. does not contain any third party
     * statement
     * @param alreadyReified reified resource
     * @throw CannotUnreify if reified statement stored in given ressource can be unreified
     */
    void ensureUnreificationIsPossible(const std::shared_ptr<Resource>& alreadyReified);

    /**
     * Unreify statement stored in Resource, and restore it as plain statement if keep is true
     * @param reifiedStatement reified statement
     * @param keep if true, we restore statement as
     * @throw CannotUnreify if reified statement stored in given ressource can be unreified
     */
    bool unReify(std::shared_ptr<Resource> reifiedStatement, bool keep);

    /**
     * @return Node from current object
     */
    Node currentNode() const {
        Node object;
        if ( _r.type() == NodeType::BLANK ) {
            object.setBNodeId(_r.name());
        } else {
            object.setIri(_r.name());
        }
        return object;
    }

    /**
     * Returns the associated factory, or throws if nullptr
     */
    static Factory *factory();

    /**
     * Full namespace for RDF, including #
     */
    static const std::string AUTORDF_NS;
    /**
     * IRI for AutoRDF index extension
     */
    static const std::string AUTORDF_ORDER;
};

/**
 * Writes object in json format.
 *
 * Is equivalent to printStream(os, 0, 0)
 */
std::ostream& operator<<(std::ostream& os, const Object&);
}

#endif //AUTORDF_OBJECT_H
