#include <autordf/StatementList.h>

#include <stdexcept>
#include <sstream>
#include <iostream>
#include <set>

#include <autordf/Resource.h>
#include <autordf/Factory.h>
#include <autordf/Exception.h>

namespace autordf {

void Resource::setType(NodeType t) {
    if ( t == NodeType::RESOURCE || t == NodeType::BLANK ) {
        _type = t;
    } else {
        throw InternalError(std::string("Node type ") + nodeTypeString(t) + " is not allowed for createBlankNodeResource");
    }
}

/**
 * Returns exactly one property, ia available.
 * @throws If not found, returns null
 */
std::shared_ptr<Property> Resource::getOptionalProperty(const std::string& iri) const {
    auto list = getPropertyValues(iri);
    if ( list.size() == 1 ) {
        return std::make_shared<Property>(list.front());
    } else if ( list.size() == 0 ) {
        return std::shared_ptr<Property>(nullptr);
    } else {
        std::stringstream ss;
        ss << "Property " << iri << " multi instanciated for " << name() << " resource." << std::endl;
        throw DuplicateProperty(ss.str());
    }
}

/**
 * Returns exactly one property.
 * @throws If more are available, throws DuplicatePropertyException
 *         If no available, throws PropertyNotFoundException
 */
Property Resource::getProperty(const std::string& iri) const {
    auto list = getPropertyValues(iri);
    if ( list.size() == 1 ) {
        return list.front();
    }
    else if ( list.empty() ) {
        std::stringstream ss;
        ss << "Property " << iri << " not found in " << name() << " resource." << std::endl;
        throw PropertyNotFound(ss.str());
    } else {
        std::stringstream ss;
        ss << "Property " << iri << " multi instanciated for " << name() << " resource." << std::endl;
        throw DuplicateProperty(ss.str());
    }
}

/**
 * Lists all values for property matching iri name
 */
std::list<Property> Resource::getPropertyValues(const std::string& iri) const {
    Statement request;
    if ( type() == NodeType::RESOURCE ) {
        request.subject.setIri(name());
    } else {
        request.subject.setBNodeId(name());
    }

    if ( !iri.empty() ) {
        request.predicate.setIri(iri);
    }
    StatementList foundTriples = _factory->find(request);

    std::list<Property> resp;
    for (const Statement& triple: foundTriples) {
        const Node& object = triple.object;
        const Node& predicate = triple.predicate;
        Property p = _factory->createProperty(predicate.iri(), object.type);
        if ( object.type == NodeType::LITERAL) {
            p.setValue(object.literal(), false);
        } else if ( object.type == NodeType::RESOURCE) {
            p.setValue(object.iri(), false);
        } else if ( object.type == NodeType::BLANK) {
            p.setValue(object.bNodeId(), false);
        }
        resp.push_back(p);
    }
    return resp;
}

void Resource::propertyAsNode(const Property& p, Node *n) {
    switch(p.type()) {
        case NodeType::RESOURCE:
            n->setIri(p.value());
            break;
        case NodeType::BLANK:
            n->setBNodeId(p.value());
            break;
        case NodeType::LITERAL:
            n->setLiteral(p.value());
            break;
        case NodeType::EMPTY:
            throw InvalidNodeType("Unable to add an Empty property!");
    }
}

Resource& Resource::addProperty(const Property &p) {
    Statement addreq;
    if ( type() == NodeType::RESOURCE ) {
        addreq.subject.setIri(name());
    } else {
        addreq.subject.setBNodeId(name());
    }
    addreq.predicate.setIri(p.iri());
    propertyAsNode(p, &addreq.object);
    _factory->add(addreq);
    return *this;
}

Resource& Resource::removeSingleProperty(const Property &p) {
    Statement rmreq;
    if ( type() == NodeType::RESOURCE ) {
        rmreq.subject.setIri(name());
    } else {
        rmreq.subject.setBNodeId(name());
    }
    rmreq.predicate.setIri(p.iri());
    propertyAsNode(p, &rmreq.object);
    _factory->remove(rmreq);
    return *this;
}

Resource& Resource::removeProperties(const std::string &iri) {
    Statement request;
    if ( type() == NodeType::RESOURCE ) {
        request.subject.setIri(name());
    } else {
        request.subject.setBNodeId(name());
    }

    if ( !iri.empty() ) {
        request.predicate.setIri(iri);
    }
    StatementList foundTriples = _factory->find(request);

    std::list<Property> resp;
    for (const Statement& triple: foundTriples) {
        _factory->remove(triple);
    }
    return *this;
}

Resource& Resource::setProperties(const std::list<Property>& list) {
    std::set<std::string> removedProps;
    for (const Property& p: list) {
        if ( !removedProps.count(p.iri()) ) {
            removeProperties(p.iri());
        }
    }
    for (const Property& p : list) {
        addProperty(p);
    }
    return *this;
}

std::ostream& operator<<(std::ostream& os, const Resource& r) {
    switch(r.type()) {
        case NodeType::RESOURCE:
            os << "R";
            break;
        case NodeType::BLANK:
            os << "B";
            break;
        default:
            os << "?";
            break;
    }
    os << "{" << r._name << "}";
    return os;
}

}
