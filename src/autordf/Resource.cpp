#include <autordf/StatementList.h>

#include <stdexcept>
#include <sstream>
#include <iostream>

#include <autordf/Resource.h>
#include <autordf/Factory.h>

namespace autordf {

void Resource::setType(NodeType t) {
    if ( t == NodeType::RESOURCE || t == NodeType::BLANK ) {
        _type = t;
    } else {
        throw std::runtime_error(std::string("Node type ") + nodeTypeString(t) + " is not allowed for Resource");
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
        throw std::runtime_error(ss.str());
    }
}

/**
 * Returns exactly one property.
 * @throws If more are available, throws DuplicateException
 *         If no available, throws NotFoundException
 */
Property Resource::getProperty(const std::string& iri) const {
    auto list = getPropertyValues(iri);
    if ( list.size() == 1 ) {
        return list.front();
    }
    else if ( list.empty() ) {
        std::stringstream ss;
        ss << "Property " << iri << " not found in " << name() << " resource." << std::endl;
        throw std::runtime_error(ss.str());
    } else {
        std::stringstream ss;
        ss << "Property " << iri << " multi instanciated for " << name() << " resource." << std::endl;
        throw std::runtime_error(ss.str());
    }
}

/**
 * Lists all values for property matching iri name
 */
std::list<Property> Resource::getPropertyValues(const std::string& iri) const {
    if ( name().empty() ) {
        throw std::runtime_error("Cannot get Property on resource with empty name");
    }

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

void Resource::addProperty(const Property &p) {
    Statement addreq;
    if ( type() == NodeType::RESOURCE ) {
        addreq.subject.setIri(name());
    } else {
        addreq.subject.setBNodeId(name());
    }
    addreq.predicate.setIri(p.iri());
    switch(p.type()) {
        case NodeType::RESOURCE:
            addreq.object.setIri(p.value());
            break;
        case NodeType::BLANK:
            addreq.object.setBNodeId(p.value());
            break;
        case NodeType::LITERAL:
            addreq.object.setLiteral(p.value());
            break;
        case NodeType::EMPTY:
            throw std::runtime_error("Unable to add an Empty property!");
    }
    _factory->add(addreq);
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
