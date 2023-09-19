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
#include <optional>

#include <autordf/Factory.h>
#include <autordf/PropertyValue.h>
#include <autordf/I18StringVector.h>
#include <autordf/Resource.h>
#include <autordf/Exception.h>
#include <autordf/Uri.h>

#include <autordf/autordf_export.h>

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
    AUTORDF_EXPORT static const std::string RDF_NS;

    /**
     * Well known RDF identifier
     */
    AUTORDF_EXPORT static const std::string RDF_TYPE;
    /**
     * Well known RDF identifier
     */
    AUTORDF_EXPORT static const std::string RDF_STATEMENT;
    /**
     * Well known RDF identifier
     */
    AUTORDF_EXPORT static const std::string RDF_SUBJECT;
    /**
     * Well known RDF identifier
     */
    AUTORDF_EXPORT static const std::string RDF_PREDICATE;
    /**
     * Well known RDF identifier
     */
    AUTORDF_EXPORT static const std::string RDF_OBJECT;

    /**
     * All newly Objects, and new values will be created in this Factory
     */
    AUTORDF_EXPORT static void setFactory(Factory *f);

    /**
     * All newly Objects, and new values will be created in this Factory.
     * Call popFactory() to restore previously set factory
     */
    AUTORDF_EXPORT static void pushFactory(Factory *f);

    /**
     * All newly Objects, and new values will be created in this Model
     */
    AUTORDF_EXPORT static void popFactory();

    /**
     * Allow to know if a factory is already loaded
     */
    AUTORDF_EXPORT static bool isFactoryLoaded();

    /**
     * Creates new object, to given iri.
     * @param iri object IRI. If empty, creates an anonymous (aka blank) object
     * @param rdfTypeIRI If not empty, will write rdf type property when object is written
     * not empty
     * @param f : if provided, work on this factory object.
     */
    AUTORDF_EXPORT Object(const Uri& iri = "", const Uri& rdfTypeIRI = "", Factory* f = nullptr);

    /**
     * Build us using the same underlying resource as the other object
     *
     * This constructs a new C++ object that points to the same RDF data
     *
     * @param obj the object to use
     */
    AUTORDF_EXPORT Object(const Object& obj);

    /**
     * Return object iri, or empty if it is a blank node
     */
    AUTORDF_EXPORT Uri iri() const;

    /**
     * Return object Qualified Name, or empty if it is a blank node
     * If a prefix is defined for this iri in the underlying model, return a Qualified Name
     * e.g. http://my/horribly/long/iri#node --> my:node
     *
     * BEWARE: prefixed IRIs should only be used for display purpose, all AutoRDF internals
     * rely on full IRIs.
     */
    AUTORDF_EXPORT std::string QName() const;

    /**
     * Return types of current object.
     * If namespaceFilter is set, only types in this namespace will be returned
     * @param namespaceFilter A simple namespace
     */
    AUTORDF_EXPORT std::vector<Uri> getTypes(const std::string& namespaceFilter = "") const;

    /**
     * Gets given property as Object
     * Property should be set.
     * If property is instanciated more than one, return one of the values, with no particular rule
     * @throw PropertyNotFound if property not set
     */
    AUTORDF_EXPORT Object getObject(const Uri& propertyIRI) const;

    /**
     * Returns given property as Object.
     * If property is instanciated more than one, return one of the values, with no particular rule
     * @returns pointer if property exists, null otherwise
     */
    AUTORDF_EXPORT std::optional<Object> getOptionalObject(const Uri& propertyIRI) const;

    /**
     * Returns the list of object. If no object found returns empty vector
     * @param propertyIRI Internationalized Resource Identifiers property to get
     * @param preserveOrdering if true, order of values stored in RDF model will be used to restore original saving order
     * @throw CannotPreserveOrder if preserveOrdering is set and ordering data is not found in model
     */
    AUTORDF_EXPORT std::vector<Object> getObjectList(const Uri& propertyIRI, bool preserveOrdering) const;

    /**
     * Sets object to given property replacing existing value
     * @param propertyIRI Internationalized Resource Identifiers property to set value of
     */
    AUTORDF_EXPORT void setObject(const Uri& propertyIRI, const Object& obj);

    /**
     * Adds object to given property
     * @param propertyIRI Internationalized Resource Identifiers property to set
     * @param obj object to add to the propertyIRI property
     * @param preserveOrdering if true, order of values will be store in RDF model. This is a non standard AutoRDF extension.
     */
    AUTORDF_EXPORT void addObject(const Uri& propertyIRI, const Object& obj, bool preserveOrdering);

    /**
     * Sets list of objects to given property
     * @param propertyIRI Internationalized Resource Identifiers property to set value of
     * @param values list of values
     * @param preserveOrdering if true, order of values will be store in RDF model. This is a non standard AutoRDF extension.
     */
    AUTORDF_EXPORT void setObjectList(const Uri& propertyIRI, const std::vector<Object>& values, bool preserveOrdering);

    /**
     * Remove the first value matching val for this object property
     * @param propertyIRI Internationalized Resource Identifiers property to remove one value from
     * @param obj object to remove from property value
     * @throw PropertyNotFound if propertyIRI has not obj as value
     */
    AUTORDF_EXPORT void removeObject(const Uri& propertyIRI, const Object& obj);

    /**
     * Gets given property value
     * Property should be set.
     * If property is instanciated more than one, return one of the values, with no particular rule
     * @param propertyIRI Internationalized Resource Identifiers property to get
     * @param f : if provided, work on this factory object.
     * @throw PropertyNotFound if property not set
     */
    AUTORDF_EXPORT PropertyValue getPropertyValue(const Uri& propertyIRI, Factory *f = nullptr) const;

    /**
     * Returns given property as Object.
     * If property is instanciated more than one, return one of the values, with no particular rule
     * @param propertyIRI Internationalized Resource Identifiers property to query
     * @param f : if provided, work on this factory object.
     * @returns pointer if property exists, null otherwise
     */
    AUTORDF_EXPORT std::optional<PropertyValue> getOptionalPropertyValue(const Uri& propertyIRI, Factory *f = nullptr) const;

    /**
     * Returns the list of the values. If no value are found returns empty list
     * @param propertyIRI Internationalized Resource Identifiers property to query
     * @param preserveOrdering if true, order of values stored in RDF model will be used to restore original saving order
     * @throw CannotPreserveOrder if preserveOrdering is set and ordering data is not found in model
     */
    AUTORDF_EXPORT std::vector<PropertyValue> getPropertyValueList(const Uri& propertyIRI, bool preserveOrdering) const;

    /**
     * Erases all previous values for property, and write unique value on place
     * @param propertyIRI Internationalized Resource Identifiers property to set
     * @param val unique value for property
     * @throw CannotUnreify if this property already has a reified a value with additional data
     */
    AUTORDF_EXPORT void setPropertyValue(const Uri& propertyIRI, const PropertyValue& val);

    /**
     * Erases all previous values for property, and write value list on place
     * @param propertyIRI Internationalized Resource Identifiers property to set
     * @param values the list of values. All previous values are removed, and replaced with the given lists
     * @param preserveOrdering if true, order of values will be store in RDF model. This is a non standard AutoRDF extension.
     */
    AUTORDF_EXPORT void setPropertyValueList(const Uri& propertyIRI, const std::vector<PropertyValue>& values, bool preserveOrdering);

    /**
     * Adds value to this property, preserving all previous values;
     * @param propertyIRI Internationalized Resource Identifiers property to set
     * @param val value for property. This new value is added to the list of values for the Property
     * @param preserveOrdering if true, order of values will be store in RDF model. This is a non standard AutoRDF extension.
     */
    AUTORDF_EXPORT void addPropertyValue(const Uri& propertyIRI, const PropertyValue& val, bool preserveOrdering);

    /**
     * Remove the first value matching val for this property
     * @param propertyIRI Internationalized Resource Identifiers property to remove one value from
     * @param val value to remove
     * @throw PropertyNotFound if propertyIRI has not val as value
     * @throw CannotUnreify if value is store as a reified statement and statement cannot be unreified
     */
    AUTORDF_EXPORT void removePropertyValue(const Uri& propertyIRI, const PropertyValue& val);

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
    AUTORDF_EXPORT Object reifyPropertyValue(const Uri& propertyIRI, const PropertyValue& val);

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
    AUTORDF_EXPORT bool unReifyPropertyValue(const Uri& propertyIRI, const PropertyValue& val, bool keep = true);

    /**
     * Test if data property value is stored as a RDF reified form, or simple statement (default)
     * @param propertyIRI Internationalized Resource Identifiers of data property
     * @param val value to test
     * @return Object representing the reified statement if available, otherwise nullptr
     */
    AUTORDF_EXPORT std::shared_ptr<Object> reifiedPropertyValue(const Uri& propertyIRI, const PropertyValue& val) const;

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
    AUTORDF_EXPORT Object reifyObject(const Uri& propertyIRI, const Object& object);

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
    AUTORDF_EXPORT bool unReifyObject(const Uri& propertyIRI, const Object& object, bool keep = true);

    /**
     * Test if object property value is stored as a RDF reified form, or simple statement (default)
     * @param propertyIRI Internationalized Resource Identifiers of data property
     * @param object value to test
     * @return Object representing the reified statement if available, otherwise nullptr
     */
    AUTORDF_EXPORT std::shared_ptr<Object> reifiedObject(const Uri& propertyIRI, const Object& object) const;

    /**
     * Forces the underlying resource to the type associated with this object.
     * This method can be called more than once, it effectively writes the type to the triple store only the first time it
     * is called.
     * This method is called by all setters of Object class
     * @return current object
     */
    AUTORDF_EXPORT Object& writeRdfType();

    /**
     * Returns true if this object is also of the specified type IRI.
     * @return true If object type attribute contains typeIRI
     * @return false If type attribute does not contains typeIRI
     */
    AUTORDF_EXPORT bool isA(const Uri& typeIRI) const;

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
     * If called with bRecursive param as false this only deletes properties and links to other objects,
     * If called with bRecursive param as true will deletes properties and also remove the bnodes
     * referenced by this object (if not referenced by other objects).
     */
    AUTORDF_EXPORT void remove(bool bRecursive = false);

    /**
     * Copies this object, to given iri.
     *
     * Object is copied by duplicating all it properties values.
     * @param iri if empty, creates an anonymous (aka blank) object.
     */
    AUTORDF_EXPORT Object clone(const Uri& iri = "") const;

    /**
     * Returns all Objects matching type specified as IRI
     *
     * @param typeIRI Internationalized Resource Identifiers of the type to be retrieved
     */
    AUTORDF_EXPORT static std::vector<Object> findByType(const Uri& typeIRI = "");

    /**
     * Returns all Objects matching any of the specified type IRI
     *
     * @param typeIRI set of Internationalized Resource Identifiers of the type to be retrieved
     */
    AUTORDF_EXPORT static std::set<Object> findByType(const std::set<Uri>& typeIRI);

    /**
     * Returns the only Object with property key set to value
     *
     * @param propertyIRI Internationalized Resource Identifier for the property
     * @param value value to look for
     *
     * @throw DuplicateObject if more than one object have the same property value
     * @throw ObjectNotFound if no object has given property with value
     */
    AUTORDF_EXPORT static Object findByKey(const Uri& propertyIRI, const PropertyValue& value);

    /**
     * Returns the only Object with property key set to value
     *
     * @param propertyIRI Internationalized Resource Identifier for the property
     * @param object resource to look for
     *
     * @throw DuplicateObject if more than one object have the same property value
     * @throw ObjectNotFound if no object has given property with value
     */
    AUTORDF_EXPORT static Object findByKey(const Uri& propertyIRI, const Object& object);

    /**
     * Returns all subjects with property set to value
     *
     * @param propertyIRI predicate IRI
     * @param value value to look for
     */
     AUTORDF_EXPORT static std::set<Object> findByValue(const Uri& propertyIRI, const PropertyValue& value);

    /**
     * Returns all subjects with property propertyIri set to iriValue
     *
     * @param propertyIRI predicate IRI
     * @param value uri to look for
     */
     AUTORDF_EXPORT static std::set<Object> findByValue(const Uri& propertyIRI, const Uri& iriValue);

    /**
     * Dumps objects content to stream
     * @param recurse: if > 0, go down resource hierarchy by recurse level
     * @param indentLevel: How much layer of tabs to insert
     */
    AUTORDF_EXPORT std::ostream& printStream(std::ostream&, int recurse = 0, unsigned int indentLevel = 0) const;

    /**
     * @param f : if provided, work on this factory object.
     * Returns all Objects in the factory
     */
    AUTORDF_EXPORT static std::set<Object> findAll(Factory* f = nullptr);


    /**
     * Find all objects pointing to the current object
     * @return
     */
    AUTORDF_EXPORT std::set<Object> findSources() const;

    /**
     * Find all objects current object is pointing out
     * @return
     */
    AUTORDF_EXPORT std::set<Object> findTargets() const;

    /**
     * Returns all reified Object properties for current object
     * @return
     */
    std::set<Object> findReified() const;

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
    template<typename T> static std::vector<T> findHelper(const Uri& iri, Factory* f = nullptr) {
        if(nullptr == f) {
            f = factory();
        }
        Statement query;
        query.predicate.setIri(RDF_NS + "type");
        query.object.setIri(iri);
        const StatementList& statements = f->find(query);
        std::vector<T> objList;
        for(const Statement& stmt : statements) {
            objList.push_back(T(f->createResourceFromNode(stmt.subject)));
        }
        return objList;
    }

    /**
     * Find all objects with the given iri
     */
    static std::set<autordf::Object> findByIri(const Uri& iri, Factory* f = nullptr) {
        if(nullptr == f) {
            f = factory();
        }
        std::set<autordf::Object> objList;

        /* Search on the subjects. */
        Statement subjectQuery;
        subjectQuery.subject.setIri(iri);
        for(const Statement& stmt : f->find(subjectQuery)) {
            objList.insert(f->createResourceFromNode(stmt.subject));
        }

        /* Search on the objects. */
        Statement objectQuery;
        objectQuery.object.setIri(iri);
        for(const Statement& stmt : f->find(objectQuery)) {
            objList.insert(f->createResourceFromNode(stmt.object));
        }

        return objList;
    }

    /**
     * Offered to interfaces
     * @throw InvalidIRI if propertyIRI is empty
     */
    template<typename T> std::vector<T> getObjectListImpl(const Uri& propertyIRI, bool preserveOrdering) const {
        std::vector<T> objList;
        propertyIterate(propertyIRI, preserveOrdering, [&objList](const Property& prop) {
            objList.emplace_back(T(prop.asResource()));
        });
        return objList;
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
    template<cvt::RdfTypeEnum rdftype, typename T> std::vector<T> getValueListImpl(const Uri& propertyIRI, bool preserveOrdering) const {
        std::vector<T> valuesList;
        propertyIterate(propertyIRI, preserveOrdering, [&valuesList](const Property& prop) {
            valuesList.emplace_back(prop.value().get<rdftype, T>());
        });
        return valuesList;
    }

    /**
     * Offered to interfaces
     * @throw InvalidIRI if propertyIRI is empty
     */
    template<cvt::RdfTypeEnum rdftype, typename T> void setValueListImpl(const Uri& propertyIRI, const std::vector<T>& values, bool preserveOrdering) {
        writeRdfType();
        removeAllReifiedDataPropertyStatements(propertyIRI);
        std::shared_ptr<Property> p = factory()->createProperty(propertyIRI);
        _r.removeProperties(propertyIRI);
        if ( !preserveOrdering ) {
            for (auto const& val: values) {
                p->setValue(PropertyValue().set<rdftype>(val));
                _r.addProperty(*p);
            }
        } else {
            long long i = 1;
            for (auto const& val: values) {
                Resource reified = createReificationResource(propertyIRI, PropertyValue().set<rdftype>(val));
                std::shared_ptr<Property> order = factory()->createProperty(AUTORDF_ORDER);
                order->setValue(PropertyValue().set<cvt::RdfTypeEnum::xsd_integer>(i));
                reified.addProperty(*order);
                ++i;
            }
        }
    }

    /**
     * Make a deep copy of an object. We stop on objects that do have an IRI because they are ressources, which we
     * do not want to make deep copies of, only shallow ones
     * @param newIri the iri under which we want to place the new object. By default, empty
     * @param doNotClone a function that takes in argument :
     *  - currentObject, the current object
     *  - sourcePredicateIri, the iri of the predicate that lead to it from its parent (if applicable)
     *  - parentObject, the object's parent (if applicable)
     *  , and returns if we shouldn't clone this object
     * @return the cloned Object
     */
    Object cloneRecursiveStopAtResources(const Uri& newIri = "", bool(*doNotClone)(const Object &currentObject, const std::string &sourcePredicateIri, const Object *parentObject) = nullptr) const;

    /**
    * Create an Object directly from a Node.
    * @param node The node to create the Object from
    */
    static Object createFromNode(const Node& node);

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

    AUTORDF_EXPORT Object(Resource r, const Uri& rdfTypeIRI = "");

    /**
     * Shared constructor
     */
    void construct(const Uri& rdfTypeIRI);

    /**
     * Iterate over all values of a given property
     */
    AUTORDF_EXPORT void propertyIterate(const Uri& propertyIRI, bool preserveOrdering, std::function<void (const Property& prop)> cb) const;

    /**
     * Creates a new resource that stores a value for this resource
     */
    AUTORDF_EXPORT Resource createReificationResource(const Uri& propertyIRI, const PropertyValue& val);

    /**
     * Creates a new resource that stores a value for this resource
     */
    AUTORDF_EXPORT Resource createReificationResource(const Uri& propertyIRI, const Resource& val);

    /**
     * Return all reified values for given property
     */
    void reifiedPropertyIterate(const Uri& propertyIRI, std::function<void (const Property& prop)> cb) const;

    /**
     * Returns the first found reified PropertyValue, if found
     */
    std::optional<PropertyValue> reifiedPropertyValueOptional(const Uri& propertyIRI, autordf::Factory *f = nullptr) const;

    /**
     * Returns the first found reified object, if found
     */
    std::optional<Object> reifiedObjectOptional(const Uri& propertyIRI) const;

    /**
     * Removes all reified statements
     * @throw CannotUnreify if one statement cannot be unreified
     */
    AUTORDF_EXPORT void removeAllReifiedObjectPropertyStatements(const Uri& propertyIRI);

    /**
     * Removes all reified statements
     * @throw CannotUnreify if one statement cannot be unreified
     */
    AUTORDF_EXPORT void removeAllReifiedDataPropertyStatements(const Uri& propertyIRI);

    /**
     * @internal
     */
    NodeList reificationResourcesForCurrentObject(autordf::Factory *f = nullptr) const;

    /**
     * Test if p is stored as a RDF reified form, or simple statement (default)
     * @param p property to test
     * @return Resource representing the reified statement if available, otherwise nullptr
     */
    AUTORDF_EXPORT std::shared_ptr<Resource> reifiedPropertyAsResource(const Property& p) const;

    /**
     * Test if val is stored as a RDF reified form, or simple statement (default)
     * @param propertyIRI Internationalized Resource Identifiers of data property
     * @param val value to test
     * @return Object representing the reified statement if available, otherwise nullptr
     */
    std::shared_ptr<Resource> reifiedPropertyValueAsResource(const Uri& propertyIRI, const PropertyValue& val) const {
        return reifiedPropertyAsResource(factory()->createProperty(propertyIRI)->setValue(val));
    }

    /**
     * Test if val is stored as a RDF reified form, or simple statement (default)
     * @param propertyIRI Internationalized Resource Identifiers of object property
     * @param val value to test
     * @return Object representing the reified statement if available, otherwise nullptr
     */
    std::shared_ptr<Resource> reifiedObjectAsResource(const Uri& propertyIRI, const Object& val) const {
        return reifiedPropertyAsResource(factory()->createProperty(propertyIRI)->setValue(val._r));
    }

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
     * An internal helper to copy properties from the current object to another
     * This ignores reification specific properties (subject, predicate, object, type) because they must be handled in
     * a more specific manner
     * @param doNotClone a function that takes in argument :
     *  - currentObject, the current object
     *  - sourcePredicateIri, the iri of the predicate that lead to it from its parent (if applicable)
     *  - parentObject, the object's parent (if applicable)
     *  , and returns if we shouldn't clone this object
     * @param to Object to which the properties are to be copied
     * @param first is this the first object in the cloning?
     * This is used to check that the first object is okay to clone - others will be checked before the clone is called
     */
    void copyPropertiesInternal( bool(*doNotClone)(const Object &currentObject, const std::string &sourcePredicateIri, const Object *parentObject), Object& to, bool first = false) const;

    /**
     * An internal helper, copies the reified properties of an object
     * @param doNotClone a function that takes in argument :
     *  - currentObject, the current object
     *  - sourcePredicateIri, the iri of the predicate that lead to it from its parent (if applicable)
     *  - parentObject, the object's parent (if applicable)
     *  , and returns if we shouldn't clone this object
     * @param to object to copy the reifications to
     */
    void copyReifiedProperties( bool(*doNotClone)(const Object &currentObject, const std::string &sourcePredicateIri, const Object *parentObject), Object& to) const;

    /**
     * Internal helper to do recursive cloning, allows us to skip
     * @param newIri see wrapper cloneRecursiveStopAtResources
     * @param doNotClone see wrapper cloneRecursiveStopAtResources
     * @param first is this the first object in the cloning?
     * This is used to check that the first object is okay to clone - others will be checked before the clone is called
     * @return
     */
    Object cloneRecursiveStopAtResourcesInternal(const Uri& newIri, bool(*doNotClone)(const Object &currentObject, const std::string &sourcePredicateIri, const Object *parentObject), bool first = false) const;
    /**
     * Returns the associated factory, or throws if nullptr
     */
    AUTORDF_EXPORT static Factory *factory();

    /**
     * Full namespace for RDF, including #
     */
    static const std::string AUTORDF_NS;
    /**
     * IRI for AutoRDF index extension
     */
    AUTORDF_EXPORT static const std::string AUTORDF_ORDER;
};

/**
 * Writes object in json format.
 *
 * Is equivalent to printStream(os, 0, 0)
 */
std::ostream& operator<<(std::ostream& os, const Object&);
}

#endif //AUTORDF_OBJECT_H
