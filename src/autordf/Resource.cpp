#include <autordf/StatementList.h>

#include <stdexcept>
#include <sstream>
#include <set>

#include <autordf/Resource.h>
#include <autordf/Factory.h>
#include <autordf/Exception.h>

namespace autordf {

void Resource::setType(NodeType t) {
    if ( t == NodeType::RESOURCE || t == NodeType::BLANK ) {
        _type = t;
    } else {
        throw InternalError(std::string("Node type ") + nodeTypeString(t) + " is not allowed for Resource");
    }
}

/**
 * @returns true if property is found, with given value
 */
bool Resource::hasProperty(const Property& p) const {
    Node subject, predicate;
    if ( type() == NodeType::RESOURCE ) {
        subject.setIri(name());
    } else {
        subject.setBNodeId(name());
    }

    if ( p.iri().empty() ) {
        throw InternalError("Not supported");
    }
    predicate.setIri(p.iri());

    Node object = _factory->findTarget(subject, predicate);
    if ( object.type() == NodeType::LITERAL ) {
        return PropertyValue(object.literal(), object.lang(), object.dataType()) == p.value();
    } else if ( object.type() == NodeType::RESOURCE ) {
        return p.asResource().name() == object.iri();
    }  else if ( object.type() == NodeType::BLANK ) {
        return p.asResource().name() == object.bNodeId();
    }
    return false;
}

/**
 * Returns exactly one property, ia available.
 * @throws If not found, returns null
 */
std::shared_ptr<Property> Resource::getOptionalProperty(const Uri& iri, Factory *f) const {
    Node subject, predicate;

    if(nullptr == f) {
        f = _factory;
    }

    if ( type() == NodeType::RESOURCE ) {
        subject.setIri(name());
    } else {
        subject.setBNodeId(name());
    }

    if ( iri.empty() ) {
        throw InternalError("Not supported");
    }
    predicate.setIri(iri);

    Node object = f->findTarget(subject, predicate);
    if ( object.empty() ) {
        return nullptr;
    }

    std::shared_ptr<Property> p(f->createProperty(predicate.iri(), object.type()));
    if (object.type() == NodeType::LITERAL) {
        p->setValue(PropertyValue(object.literal(), object.lang(), object.dataType()), false);
    } else if (object.type() == NodeType::RESOURCE) {
        p->setValue(PropertyValue(object.iri(), datatype::DATATYPE_STRING), false);
    } else if (object.type() == NodeType::BLANK) {
        p->setValue(PropertyValue(object.bNodeId(), datatype::DATATYPE_STRING), false);
    }
    return p;
}

/**
 * Returns exactly one property.
 * @throws If more are available, throws DuplicatePropertyException
 *         If no available, throws PropertyNotFoundException
 */
std::shared_ptr<Property> Resource::getProperty(const Uri& iri) const {
    auto p = getOptionalProperty(iri);
    if ( !p ) {
        std::stringstream ss;
        ss << "Property " << iri << " not found in " << name() << " resource." << std::endl;
        throw PropertyNotFound(ss.str());
    }
    return p;
}

/**
 * Lists all values for property matching iri name
 */
std::shared_ptr<std::list<Property>> Resource::getPropertyValues(const Uri& iri) const {
    Node subject, predicate;
    if ( type() == NodeType::RESOURCE ) {
        subject.setIri(name());
    } else {
        subject.setBNodeId(name());
    }

    if ( iri.empty() ) {
        throw InternalError("getPropertyValues(const Uri& iri): iri cannot be empty");
    }
    predicate.setIri(iri);

    NodeList foundTriples = _factory->findTargets(subject, predicate);
    auto resp = std::make_shared<std::list<Property>>();
    for (const Node& object: foundTriples) {
        std::shared_ptr<Property> p = _factory->createProperty(predicate.iri(), object.type());
        if ( object.type() == NodeType::LITERAL) {
            p->setValue(PropertyValue(object.literal(), object.lang(), object.dataType()), false);
        } else if ( object.type() == NodeType::RESOURCE) {
            p->setValue(PropertyValue(object.iri(), datatype::DATATYPE_STRING), false);
        } else if ( object.type() == NodeType::BLANK) {
            p->setValue(PropertyValue(object.bNodeId(), datatype::DATATYPE_STRING), false);
        }
        resp->push_back(*p);
    }
    return resp;
}

/**
 * Lists all values for all properties
 */
std::shared_ptr<std::list<Property>> Resource::getPropertyValues() const {
    Statement request;
    if ( type() == NodeType::RESOURCE ) {
        request.subject.setIri(name());
    } else {
        request.subject.setBNodeId(name());
    }

    StatementList foundTriples = _factory->find(request);

    auto resp = std::make_shared<std::list<Property>>();
    for (const Statement& triple: foundTriples) {
        const Node& object = triple.object;
        const Node& predicate = triple.predicate;
        std::shared_ptr<Property> p = _factory->createProperty(predicate.iri(), object.type());
        if ( object.type() == NodeType::LITERAL) {
            p->setValue(PropertyValue(object.literal(), object.lang(), object.dataType()), false);
        } else if ( object.type() == NodeType::RESOURCE) {
            p->setValue(PropertyValue(object.iri(), datatype::DATATYPE_STRING), false);
        } else if ( object.type() == NodeType::BLANK) {
            p->setValue(PropertyValue(object.bNodeId(), datatype::DATATYPE_STRING), false);
        }
        resp->push_back(*p);
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
            n->setLiteral(p.value(), p.value().lang(), p.value().dataTypeIri());
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
    _factory->add(&addreq);
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

    try {
        _factory->remove(&rmreq);
    }
    catch (const InternalError &) {

        if(p.value().dataTypeIri() == autordf::datatype::DATATYPE_STRING) {

            Property p0(p);
            PropertyValue p0v = p.value();
            p0v.setDataTypeIri("");
            p0.setValue(p0v);
            propertyAsNode(p0, &rmreq.object);

            try {
                _factory->remove(&rmreq);
            }
            catch (const InternalError &) {
                throw PropertyNotFound("Unable to remove Property " + p.iri() + ", value " + p.value());
            }
        } else {
            throw PropertyNotFound("Unable to remove Property " + p.iri() + ", value " + p.value());
        }

    }

    propertyAsNode(p, &rmreq.object);

    return *this;
}

Resource& Resource::removeProperties(const Uri &iri) {
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

    for (Statement& triple: foundTriples) {
        _factory->remove(&triple);
    }
    return *this;
}

Resource& Resource::setProperties(const std::shared_ptr<std::list<Property>>& list) {
    std::set<std::string> removedProps;
    for (const Property& p: *list) {
        if ( !removedProps.count(p.iri()) ) {
            removeProperties(p.iri());
        }
    }
    for (const Property& p : *list) {
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
