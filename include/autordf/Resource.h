#ifndef AUTORDF_RESOURCE_H
#define AUTORDF_RESOURCE_H

#include <memory>
#include <list>
#include <iosfwd>

#include <autordf/Property.h>
#include <autordf/Uri.h>

namespace autordf {

class Node;
class Factory;

/**
 * A Resource provides accessors to list and write properties to a Web Semantic Resource
 */
class Resource {
public:
    /**
     * Only RESOURCE and BLANK are allowed on type
     */
    NodeType type() const { return _type; }

    /**
     * Can contain one of the following things
     * Nothing: We are a blank node with no generated id yet
     * an IRI: We are an IRI identified resource (type is RESOURCE)
     * a blank node id: We are a blank node (type BLANK), where we already been assigned a blank node id
     */
    const std::string& name() const { return _name; }

    /**
     * @returns true if property is found, with any value
     */
    bool hasProperty(const Uri& iri) const { return getOptionalProperty(iri).get(); }

    /**
     * @returns true if property is found, with given value
     */
    bool hasProperty(const Property& p) const;

    /**
     * Returns exactly one property.
     * @throws PropertyNotFound If no available
     * @throws DuplicateException If more than one is available
     */
    std::shared_ptr<Property> getProperty(const Uri& iri) const;

    /**
     * Returns exactly one property, ia available.
     * @throws DuplicateProperty If more than 1 instance of the property is found
     */
    std::shared_ptr<Property> getOptionalProperty(const Uri& iri) const;

    /**
     * Lists all values for property matching iri name
     * If iri is empty, matches ALL properties with ALL their values
     */
    std::shared_ptr<std::list<Property>> getPropertyValues(const Uri& iri) const;

    /**
     * Lists all values for all properties
     */
    std::shared_ptr<std::list<Property>> getPropertyValues() const;

    /**
     * Adds a property to the resource. Property should not be empty
     * @returns a reference to current object
     */
    Resource& addProperty(const Property& p);

    /** Remove all properties matching iri in the list, then
     * Add properties one by one
     * @returns a reference to current object
     */
    Resource& setProperties(const std::shared_ptr<std::list<Property>>& list);

    /**
     * Removes a single value for a property
     * @throw PropertyNotFound if property is not found
     * @returns a reference to current object
     */
    Resource& removeSingleProperty(const Property& p);

    /**
     * Remove all  properties matching iri
     * If iri is empty, remove *All* object properties
     * @returns a reference to current object
     */
    Resource& removeProperties(const Uri &iri);

    /**
     * Removes a ressource, i.e. delete all its attributes
     * @returns a reference to current object
     */
    Resource& remove() { return removeProperties(""); }

private:
    NodeType _type;

    // IRI or Blanc node id, depending on context
    std::string _name;

    Factory *_factory;

    // Should only be built through Factory
    Resource(NodeType type, const std::string& name, Factory *f) : _name(name), _factory(f) { setType(type); }

    void setType(NodeType t);

    static void propertyAsNode(const Property& p, Node *n);

    friend class Factory;
    friend std::ostream& operator<<(std::ostream& os, const Resource&);
};

std::ostream& operator<<(std::ostream& os, const Resource&);

}

#endif //AUTORDF_RESOURCE_H
