#include <autordf/Object.h>

#include <autordf/Factory.h>
#include <autordf/Exception.h>

#include <memory>

namespace autordf {

std::stack<Factory *> Object::_factories;

const std::string Object::RDF_NS = "http://www.w3.org/1999/02/22-rdf-syntax-ns#";
const std::string Object::RDF_TYPE = RDF_NS + "type";
const std::string Object::RDF_STATEMENT = RDF_NS + "Statement";
const std::string Object::RDF_SUBJECT = RDF_NS + "subject";
const std::string Object::RDF_PREDICATE = RDF_NS + "predicate";
const std::string Object::RDF_OBJECT = RDF_NS + "object";

const std::string Object::AUTORDF_NS = "http://github.com/ariadnext/AutoRDF#";
const std::string Object::AUTORDF_ORDER = AUTORDF_NS + "order";

void Object::setFactory(Factory *f) {
    if ( _factories.empty() ) {
        _factories.push(f);
    } else {
        _factories.top() = f;
    }
}

void Object::pushFactory(Factory *f) {
    _factories.push(f);
}

void Object::popFactory() {
    _factories.pop();
}

bool Object::isFactoryLoaded() {
    return !_factories.empty();
}

Object::Object(const Uri &iri, const Uri& rdfTypeIRI, Factory* f)
    : _r(iri.empty() ? (f ? f : factory())->createBlankNodeResource() : (f ? f : factory())->createIRIResource(iri)) {
    construct(rdfTypeIRI);
}

Object::Object(const Object& other) : _r(other._r), _rdfTypeWritingRequired(other._rdfTypeWritingRequired), _rdfTypeIRI(other._rdfTypeIRI) {
}

Object::Object(Resource r, const Uri& rdfTypeIRI) : _r(r) {
    construct(rdfTypeIRI);
}

void Object::construct(const Uri& rdfTypeIRI) {
    if ( !rdfTypeIRI.empty() ) {
        _rdfTypeIRI = rdfTypeIRI;
        _rdfTypeWritingRequired = true;
    } else {
        _rdfTypeWritingRequired = false;
    }
}

Uri Object::iri() const {
    if ( _r.type() == NodeType::RESOURCE ) {
        return _r.name();
    } else {
        static const Uri EMPTY("");
        return EMPTY;
    }
}

std::string Object::QName() const {
    return iri().QName(factory());
}

std::vector<Uri> Object::getTypes(const std::string& namespaceFilter) const {
    std::vector<Uri> obj;
    const std::shared_ptr<std::list<Property>>& propList = _r.getPropertyValues(RDF_TYPE);
    for (const Property& prop: *propList) {
        autordf::Uri type = prop.asResource().name();
        if (namespaceFilter.empty() || type.find(namespaceFilter) == 0) {
            obj.push_back(type);
        }
    }
    return obj;
}

Object Object::getObject(const Uri &propertyIRI) const {
    std::shared_ptr<Object> obj = getOptionalObject(propertyIRI);
    if ( obj ) {
        return *obj;
    } else {
        std::stringstream ss;
        ss << "Property " << propertyIRI << " not found in " << _r.name() << " resource." << std::endl;
        throw PropertyNotFound(ss.str());
    }
}

std::shared_ptr<Object> Object::getOptionalObject(const Uri& propertyIRI) const {
    std::shared_ptr<Property> p(_r.getOptionalProperty(propertyIRI));
    if ( p ) {
        return std::shared_ptr<Object>(new Object(p->asResource()));
    } else {
        std::shared_ptr<Object> reified = reifiedObjectOptional(propertyIRI);
        if ( reified ) {
            return reified;
        } else {
            return nullptr;
        }
    }
}

std::vector<Object> Object::getObjectList(const Uri& propertyIRI, bool preserveOrdering) const {
    return getObjectListImpl<Object>(propertyIRI, preserveOrdering);
}

void Object::setObject(const Uri& propertyIRI, const Object& obj) {
    // First remove all reified statements
    removeAllReifiedObjectPropertyStatements(propertyIRI);
    writeRdfType();
    std::shared_ptr<Property> p = factory()->createProperty(propertyIRI);
    p->setValue(obj._r);
    _r.removeProperties(propertyIRI);
    _r.addProperty(*p);
}

void Object::addObject(const Uri& propertyIRI, const Object& obj, bool preserveOrdering) {
    writeRdfType();
    if ( !reifiedObjectAsResource(propertyIRI, obj) ) {
        if (!preserveOrdering) {
            _r.addProperty(factory()->createProperty(propertyIRI)->setValue(obj._r));
        } else {
            long long maxVal = 0;
            reifiedPropertyIterate(propertyIRI, [&](const Property& p) {
                std::shared_ptr<Property> orderprop = reifiedObjectAsResource(propertyIRI, p.asResource())->getOptionalProperty(AUTORDF_ORDER);
                if ( orderprop ) {
                    long long order = orderprop->value().get<cvt::RdfTypeEnum::xsd_integer, long long>();
                    maxVal = std::max(maxVal, order);
                }
            });
            Resource reified = createReificationResource(propertyIRI, obj._r);
            std::shared_ptr<Property> order = factory()->createProperty(AUTORDF_ORDER);
            order->setValue(PropertyValue().set<cvt::RdfTypeEnum::xsd_integer>(maxVal + 1));
            reified.addProperty(*order);
        }
    }
}

void Object::setObjectList(const Uri& propertyIRI, const std::vector<Object> &values, bool preserveOrdering) {
    setObjectListImpl(propertyIRI, values, preserveOrdering);
}

void Object::removeObject(const Uri& propertyIRI, const Object& obj) {
    // Check for reified object
    if ( !unReifyObject(propertyIRI, obj, false) ) {
        // No reified object to remove, use std removal
        std::shared_ptr<Property> p = factory()->createProperty(propertyIRI);
        p->setValue(obj._r);
        _r.removeSingleProperty(*p);
    }
}

PropertyValue Object::getPropertyValue(const Uri& propertyIRI, Factory *f ) const {
    std::shared_ptr<PropertyValue> pv = getOptionalPropertyValue(propertyIRI, f);
    if ( pv ) {
        return *pv;
    } else {
        std::stringstream ss;
        ss << "Property " << propertyIRI << " not found in " << _r.name() << " resource." << std::endl;
        throw PropertyNotFound(ss.str());
    }
}

std::shared_ptr<PropertyValue> Object::getOptionalPropertyValue(const Uri& propertyIRI, autordf::Factory *f ) const {

    if (nullptr == f) {
        f = factory();
    }

    std::shared_ptr<Property> p(_r.getOptionalProperty(propertyIRI, f));
    if ( p ) {
        return std::make_shared<PropertyValue>(p->value());
    } else {
        std::shared_ptr<PropertyValue> reified = reifiedPropertyValueOptional(propertyIRI,f);
        if ( reified ) {
            return reified;
        } else {
            return nullptr;
        }
    }
}

std::vector<PropertyValue> Object::getPropertyValueList(const Uri& propertyIRI, bool preserveOrdering) const {
    std::vector<PropertyValue> valuesList;
    propertyIterate(propertyIRI, preserveOrdering, [&valuesList](const Property& prop) {
        valuesList.emplace_back(prop.value());
    });
    return valuesList;
}

void Object::setPropertyValue(const Uri& propertyIRI, const PropertyValue& val) {
    // First remove all reified statements
    removeAllReifiedDataPropertyStatements(propertyIRI);
    writeRdfType();
    std::shared_ptr<Property> p = factory()->createProperty(propertyIRI);
    p->setValue(val);
    _r.removeProperties(propertyIRI);
    _r.addProperty(*p);
}

void Object::addPropertyValue(const Uri& propertyIRI, const PropertyValue& val, bool preserveOrdering) {
    writeRdfType();
    if (!reifiedPropertyValueAsResource(propertyIRI, val)) {
        if (!preserveOrdering) {
            _r.addProperty(factory()->createProperty(propertyIRI)->setValue(val));
        } else {
            long long maxVal = 0;
            reifiedPropertyIterate(propertyIRI, [&](const Property& p) {
                std::shared_ptr<Property> orderprop = reifiedPropertyAsResource(p)->getOptionalProperty(AUTORDF_ORDER);
                if ( orderprop ) {
                    long long order = orderprop->value().get<cvt::RdfTypeEnum::xsd_integer, long long>();
                    maxVal = std::max(maxVal, order);
                }
            });
            Resource reified = createReificationResource(propertyIRI, val);
            std::shared_ptr<Property> order = factory()->createProperty(AUTORDF_ORDER);
            order->setValue(PropertyValue().set<cvt::RdfTypeEnum::xsd_integer>(maxVal + 1));
            reified.addProperty(*order);
        }
    }
}

void Object::removePropertyValue(const Uri& propertyIRI, const PropertyValue& val) {
    // Check for reified object
    if ( !unReifyPropertyValue(propertyIRI, val, false) ) {
        // No reified object to remove, use std removal
        std::shared_ptr<Property> p = factory()->createProperty(propertyIRI);
        p->setValue(val);
        _r.removeSingleProperty(*p);
    }
}

Resource Object::createReificationResource(const Uri& propertyIRI, const PropertyValue& val) {
    Resource reified = factory()->createBlankNodeResource();
    reified.addProperty(factory()->createProperty(RDF_TYPE)->setValue(factory()->createIRIResource(RDF_STATEMENT)));
    reified.addProperty(factory()->createProperty(RDF_SUBJECT)->setValue(_r));
    reified.addProperty(factory()->createProperty(RDF_PREDICATE)->setValue(factory()->createIRIResource(propertyIRI)));
    reified.addProperty(factory()->createProperty(RDF_OBJECT)->setValue(val));
    return reified;
}

Resource Object::createReificationResource(const Uri& propertyIRI, const Resource& val) {
    Resource reified = factory()->createBlankNodeResource();
    reified.addProperty(factory()->createProperty(RDF_TYPE)->setValue(factory()->createIRIResource(RDF_STATEMENT)));
    reified.addProperty(factory()->createProperty(RDF_SUBJECT)->setValue(_r));
    reified.addProperty(factory()->createProperty(RDF_PREDICATE)->setValue(factory()->createIRIResource(propertyIRI)));
    reified.addProperty(factory()->createProperty(RDF_OBJECT)->setValue(val));
    return reified;
}

Object Object::reifyPropertyValue(const Uri& propertyIRI, const PropertyValue& val) {
    // Check for reified object
    std::shared_ptr<Resource> alreadyReified = reifiedPropertyValueAsResource(propertyIRI, val);

    if ( alreadyReified ) {
        return Object(*alreadyReified);
    } else {
        Resource reified = createReificationResource(propertyIRI, val);

        //TODO: we would like some kind of transactional database to prevent inconsistencies in failure case scenarios
        std::shared_ptr<Property> p = factory()->createProperty(propertyIRI);
        p->setValue(val);
        if ( _r.hasProperty(*p) ) {
            _r.removeSingleProperty(*p);
        }

        return Object(reified);
    }
}


Object Object::reifyObject(const Uri& propertyIRI, const Object& object) {
    // Check for reified object
    std::shared_ptr<Resource> alreadyReified = reifiedObjectAsResource(propertyIRI, object);

    if ( alreadyReified ) {
        return Object(*alreadyReified);
    } else {
        Resource reified = createReificationResource(propertyIRI, object._r);

        //TODO: we would like some kind of transactional database to prevent inconsistencies in failure case scenarios
        std::shared_ptr<Property> p = factory()->createProperty(propertyIRI);
        p->setValue(object._r);
        if ( _r.hasProperty(*p) ) {
            _r.removeSingleProperty(*p);
        }

        return Object(reified);
    }
}

void Object::ensureUnreificationIsPossible(const std::shared_ptr<Resource>& alreadyReified) {
    static const std::set<std::string> RDF_REIFICATION_STATEMENTS = {
            RDF_TYPE, RDF_SUBJECT, RDF_PREDICATE, RDF_OBJECT, AUTORDF_ORDER
    };
    const std::shared_ptr<std::list<Property>>& propList = alreadyReified->getPropertyValues();
    for ( const Property& prop : *propList ) {
        if ( !RDF_REIFICATION_STATEMENTS.count(prop.iri()) ) {
            std::stringstream ss;
            ss << "Reified statement contains user defined property " << prop;
            throw CannotUnreify(ss.str());
        }
    }
}

bool Object::unReify(std::shared_ptr<Resource> alreadyReified, bool keep) {
    if ( alreadyReified ) {
        ensureUnreificationIsPossible(alreadyReified);
        //TODO: we would like some kind of transactional database to prevent inconsistencies in failure case scenarios
        std::shared_ptr<Property> predicate = alreadyReified->getProperty(RDF_PREDICATE);
        std::shared_ptr<Property> object = alreadyReified->getProperty(RDF_OBJECT);

        if ( keep ) {
            // Add unreified statement
            std::shared_ptr<Property> prop = factory()->createProperty(predicate->asResource().name(), object->type());
            prop->setValue(object->value());
            _r.addProperty(*prop);
        }

        // Remove reified statement
        alreadyReified->remove();

        return true;
    } else {
        return false;
    }
}

bool Object::unReifyObject(const Uri& propertyIRI, const Object& object, bool keep) {
    return unReify(reifiedObjectAsResource(propertyIRI, object), keep);
}

bool Object::unReifyPropertyValue(const Uri& propertyIRI, const PropertyValue& val, bool keep) {
    return unReify(reifiedPropertyValueAsResource(propertyIRI, val), keep);
}

NodeList Object::reificationResourcesForCurrentObject(Factory *f ) const {
    Node predicate;

    if(nullptr == f) {
        f = factory();
    }

    predicate.setIri(RDF_SUBJECT);
    return f->findSources(predicate, currentNode());
}

std::shared_ptr<Object> Object::reifiedPropertyValue(const Uri& propertyIRI, const PropertyValue& val) const {
    std::shared_ptr<Resource> res = reifiedPropertyValueAsResource(propertyIRI, val);
    if (res) {
        return std::shared_ptr<Object>(new Object(*res));
    }
    return nullptr;
}

std::shared_ptr<Object> Object::reifiedObject(const Uri& propertyIRI, const Object& object) const {
    std::shared_ptr<Resource> res = reifiedObjectAsResource(propertyIRI, object);
    if (res) {
        return std::shared_ptr<Object>(new Object(*res));
    }
    return nullptr;
}

std::shared_ptr<Resource> Object::reifiedPropertyAsResource(const Property& p) const {
    const NodeList nodesReferingToThisObject = reificationResourcesForCurrentObject();

    // Iterate through statements and find ones matching predicate and object
    for (const Node& thisObject : nodesReferingToThisObject ) {
        Resource reifiedStatement(factory()->createResourceFromNode(thisObject));
        std::shared_ptr<Property> predicate = reifiedStatement.getProperty(RDF_PREDICATE);
        if ( predicate->asResource().name() == p.iri() ) {
            std::shared_ptr<Property> object = reifiedStatement.getProperty(RDF_OBJECT);
            switch (p.type()) {
                case NodeType::LITERAL:
                    if ( object->isLiteral() && (object->value() == p.value()) ) {
                        return std::make_shared<Resource>(reifiedStatement);
                    }
                    break;
                case NodeType::RESOURCE:
                case NodeType::BLANK:
                    if ( object->asResource().name() == p.asResource().name() ){
                        return std::make_shared<Resource>(reifiedStatement);
                    }
                    break;
                default:
                    throw InternalError("reifiedPropertyAsResource invalid property type");
            }
        }
    }
    return nullptr;
}

std::shared_ptr<PropertyValue> Object::reifiedPropertyValueOptional(const Uri& propertyIRI, autordf::Factory *f ) const {
    const NodeList nodesReferingToThisObject = reificationResourcesForCurrentObject(f);

    if(nullptr == f) {
        f = factory();
    }

    // Iterate through statements and find ones matching predicate
    std::vector<Object> allReifiedValues;
    for (const Node& thisObject : nodesReferingToThisObject ) {
        Resource reifiedStatement(f->createResourceFromNode(thisObject));
        std::shared_ptr<Property> predicate = reifiedStatement.getProperty(RDF_PREDICATE);
        if ( predicate && predicate->asResource().name() == propertyIRI ) {
            std::shared_ptr<Property> prop = reifiedStatement.getProperty(RDF_OBJECT);
            if ( prop->isLiteral() ) {
                return std::make_shared<PropertyValue>(prop->value());
            }
        }
    }
    return nullptr;
}

std::shared_ptr<Object> Object::reifiedObjectOptional(const Uri& propertyIRI) const {
    const NodeList nodesReferingToThisObject = reificationResourcesForCurrentObject();

    // Iterate through statements and find ones matching predicate
    std::vector<Object> allReifiedValues;
    for (const Node& thisObject : nodesReferingToThisObject ) {
        Resource reifiedStatement(factory()->createResourceFromNode(thisObject));
        std::shared_ptr<Property> predicate = reifiedStatement.getProperty(RDF_PREDICATE);
        if ( predicate && predicate->asResource().name() == propertyIRI ) {
            std::shared_ptr<Property> prop = reifiedStatement.getProperty(RDF_OBJECT);
            if ( prop->isResource() ) {
                return std::shared_ptr<Object>(new Object(prop->asResource()));
            }
        }
    }
    return nullptr;
}

void Object::removeAllReifiedObjectPropertyStatements(const Uri& propertyIRI) {
    reifiedPropertyIterate(propertyIRI, [this](const Property& p) {
        unReifyObject(p.iri(), p.asResource(), false);
    });
}

void Object::removeAllReifiedDataPropertyStatements(const Uri& propertyIRI) {
    reifiedPropertyIterate(propertyIRI, [this](const Property& p) {
        unReifyPropertyValue(p.iri(), p.value(), false);
    });
}

void Object::reifiedPropertyIterate(const Uri& propertyIRI, std::function<void (const Property& p)> cb) const {
    const NodeList nodesReferingToThisObject = reificationResourcesForCurrentObject();

    // Iterate through statements and find ones matching predicate
    for (const Node& thisObject : nodesReferingToThisObject ) {
        Resource reifiedStatement(factory()->createResourceFromNode(thisObject));
        std::shared_ptr<Property> predicate = reifiedStatement.getProperty(RDF_PREDICATE);
        if ( predicate && predicate->asResource().name() == propertyIRI ) {
            std::shared_ptr<Property> rdfobject = reifiedStatement.getProperty(RDF_OBJECT);
            std::shared_ptr<Property> prop = factory()->createProperty(propertyIRI, rdfobject->type());
            prop->setValue(rdfobject->value(), false);
            cb(*prop);
        }
    }
}

void Object::setPropertyValueList(const Uri& propertyIRI, const std::vector<PropertyValue>& values, bool preserveOrdering) {
    writeRdfType();
    removeAllReifiedDataPropertyStatements(propertyIRI);
    std::shared_ptr<Property> p = factory()->createProperty(propertyIRI);
    _r.removeProperties(propertyIRI);
    if ( !preserveOrdering ) {
        for (const PropertyValue& val: values) {
            p->setValue(val);
            _r.addProperty(*p);
        }
    } else {
        long long i = 1;
        for (const PropertyValue& val: values) {
            Resource reified = createReificationResource(propertyIRI, val);
            std::shared_ptr<Property> order = factory()->createProperty(AUTORDF_ORDER);
            order->setValue(PropertyValue().set<cvt::RdfTypeEnum::xsd_integer>(i));
            reified.addProperty(*order);
            ++i;
        }
    }
}

void Object::propertyIterate(const Uri& propertyIRI, bool preserveOrdering, std::function<void (const Property& prop)> cb) const {
    if ( propertyIRI.empty() ) {
        throw InvalidIRI("Calling propertyIterate() with empty IRI is forbidden");
    }
    const std::shared_ptr<std::list<Property>>& propList = _r.getPropertyValues(propertyIRI);
    unsigned int count = 0;
    for (const Property& prop: *propList) {
        cb(prop);
        ++count;
    }
    if ( !preserveOrdering ) {
        // Iterate through statements and find ones matching predicate
        reifiedPropertyIterate(propertyIRI, [cb](const Property& p) {
            cb(p);
        });
    } else {
        if ( count ) {
            throw CannotPreserveOrder("Unable to read back statements order as there is at least one unreified statement without ordering info");
        } else {
            typedef std::pair<long long, Property> PropertyWithOrder;
            std::vector<PropertyWithOrder> unordered;
            reifiedPropertyIterate(propertyIRI, [&](const Property& p) {
                std::shared_ptr<Property> orderprop = reifiedPropertyAsResource(p)->getOptionalProperty(AUTORDF_ORDER);
                if ( !orderprop ) {
                    throw CannotPreserveOrder("Unable to read back statements order as there is at least one reified statement missing ordering info");
                }
                unordered.emplace_back(std::make_pair(orderprop->value().get<cvt::RdfTypeEnum::xsd_integer, long long>(), p));
            });
            std::sort(unordered.begin(), unordered.end(), [](const PropertyWithOrder& a, const PropertyWithOrder& b) {
                return a.first < b.first;
            });
            for ( const PropertyWithOrder& pvo : unordered ) {
                cb(pvo.second);
            }
        }
    }
}

bool Object::isA(const Uri& typeIRI) const {
    return _r.isA(typeIRI);
}

void Object::remove(bool bRecursive /*= false*/) {

    if(bRecursive){
        if (isA(RDF_STATEMENT)) {
            std::shared_ptr<Property> object = _r.getProperty(RDF_OBJECT);
            if(NodeType::BLANK == object->type()){
                Object(object->asResource()).remove(bRecursive);
            }
        } else {
            const NodeList nodesReferingToThisObject = reificationResourcesForCurrentObject();
            for (const Node& thisObject : nodesReferingToThisObject ) {
                Resource reifiedStatement(factory()->createResourceFromNode(thisObject));
                std::shared_ptr<Property> rdfObj = reifiedStatement.getProperty(RDF_OBJECT);
                if ( rdfObj->type() == NodeType::BLANK) {
                    Object(rdfObj->asResource()).remove(bRecursive);
                }
                Object(reifiedStatement).remove();
            }
            Statement query;
            query.subject = currentNode();
            const StatementList &statements = factory()->find(query);
            for (const Statement &stmt: statements) {
                if (stmt.object.type() == NodeType::BLANK) {
                    Object subobj(factory()->createResourceFromNode(stmt.object));
                    if (subobj.findSources().size() == 1) {
                        subobj.remove(bRecursive);
                    }
                }
            }
        }
    }

    _r.removeProperties("");
    if ( !_rdfTypeIRI.empty() ) {
        _rdfTypeWritingRequired = true;
    }
}

Object Object::clone(const Uri& iri) const {
    const std::shared_ptr<std::list<Property>>& props = _r.getPropertyValues();
    Object res;
    if ( !iri.empty() ) {
        res = Object(factory()->createIRIResource(iri).setProperties(props), _rdfTypeIRI);
    } else {
        res = Object(factory()->createBlankNodeResource("").setProperties(props), _rdfTypeIRI);
    }
    copyReifiedProperties(nullptr, res);
    return res;
}

std::vector<Object> Object::findByType(const Uri& iri) {
    return findHelper<Object>(iri);
}

std::set<Object> Object::findByType(const std::set<Uri>& typeIRI) {
    std::set<Object> objList;

    for ( const Uri& type : typeIRI ) {
        Statement query;
        query.predicate.setIri(RDF_NS + "type");
        query.object.setIri(type);
        const StatementList& statements = factory()->find(query);
        for(const Statement& stmt : statements) {
            objList.insert(Object(factory()->createResourceFromNode(stmt.subject)));
        }
    }
    return objList;
}

Object Object::findByKey(const Uri& propertyIRI, const PropertyValue& value) {
    Statement query;
    query.predicate.setIri(propertyIRI);
    query.object.setLiteral(value, value.lang(), value.dataTypeIri());
    const StatementList& statements = factory()->find(query);
    if ( statements.empty() ) {
        throw ObjectNotFound(std::string("No object with owl key ") + propertyIRI + " set to " + value + " found");
    }
    if ( statements.size() > 1 ) {
        throw DuplicateObject(std::string("More than one object with owl key ") + propertyIRI + " set to " + value + " found");
    }
    return Object(factory()->createResourceFromNode(statements.begin()->subject));
}

Object Object::findByKey(const Uri& propertyIRI, const Object& object) {
    Statement query;
    query.predicate.setIri(propertyIRI);
    query.object.setIri(object.iri());
    const StatementList& statements = factory()->find(query);
    if ( statements.empty() ) {
        throw ObjectNotFound(std::string("No object with owl key ") + propertyIRI + " set to " + object.iri() + " found");
    }
    if ( statements.size() > 1 ) {
        throw DuplicateObject(std::string("More than one object with owl key ") + propertyIRI + " set to " + object.iri() + " found");
    }
    return Object(factory()->createResourceFromNode(statements.begin()->subject));
}

std::set<Object> Object::findByValue(const Uri& propertyIRI, const PropertyValue& value) {
    std::set<Object> objList;
    Statement query;
    query.predicate.setIri(propertyIRI);
    query.object.setLiteral(value, value.lang(), value.dataTypeIri());
    const StatementList& statements = factory()->find(query);
    for (const Statement& stmt: statements) {
        objList.insert(Object(factory()->createResourceFromNode(stmt.subject)));
    }
    return objList;
}

std::set<Object> Object::findByValue(const Uri& propertyIRI, const Uri& iriValue) {
    std::set<Object> objList;
    Statement query;
    query.predicate.setIri(propertyIRI);
    query.object.setIri(iriValue);
    const StatementList& statements = factory()->find(query);
    for (const Statement& stmt: statements) {
        objList.insert(Object(factory()->createResourceFromNode(stmt.subject)));
    }
    return objList;
}

Object& Object::writeRdfType() {
    if ( _rdfTypeWritingRequired ) {
        _rdfTypeWritingRequired = false;
        setObject(RDF_TYPE, Object(_rdfTypeIRI));
    }
    return *this;
}

Factory *Object::factory() {
    if ( _factories.empty() ) {
        throw std::runtime_error("You must call autordf::Object::setFactory() before using any of your objects methods");
    }
    return _factories.top();
}


namespace {

void indentLine(std::ostream& os, unsigned int indent) {
    for (unsigned int i = 0; i < indent; ++i) {
        os << '\t';
    }
}

void newLine(std::ostream& os, unsigned int indent) {
    os << std::endl;
    indentLine(os, indent);
}

}

std::ostream& operator<<(std::ostream& os, const Object& obj) {
    obj.printStream(os);
    return os;
}

std::ostream& Object::printStream(std::ostream& os, int recurse, unsigned int indentLevel) const {
    indentLine(os, indentLevel);
    os << "\"Subject\": \"" << _r.name() <<'"';
    std::set<std::string> donePropsIRI;
    // Get all props
    const std::shared_ptr<std::list<Property>>& propsList = _r.getPropertyValues();
    for ( auto propit = propsList->begin(); propit != propsList->end(); ++propit ) {
        if ( !donePropsIRI.count(propit->iri()) ) {
            if ( propit != propsList->begin() ) {
                os << ',';
            }
            newLine(os, indentLevel);
            donePropsIRI.insert(propit->iri());
            os << '"' << propit->iri() << "\": ";
            if ( (recurse > 0) && (propit->type() == NodeType::BLANK || propit->type() == NodeType::RESOURCE ) && propit->asResource().getPropertyValues()->size() ) {
                Object child(propit->asResource());
                os << '{' << std::endl;
                child.printStream(os, recurse - 1, indentLevel + 1);
                newLine(os, indentLevel);
                os << '}';
            } else {
                const std::vector<PropertyValue>& values = getPropertyValueList(propit->iri(), false);
                if ( values.size() > 1 ) {
                    os << '[';
                    newLine(os, indentLevel + 1);
                    for ( auto valit = values.begin(); valit != values.end(); ++valit) {
                        if ( valit != values.begin() ) {
                            os << ',';
                            newLine(os, indentLevel + 1);
                        }
                        os << '"' << *valit << '"';
                    }
                    newLine(os, indentLevel);
                    os << ']';
                } else {
                    os << '"' << getPropertyValue(propit->iri()) << '"';
                }
            }
        }
    }
    return os;
}

std::set<Object> Object::findAll(Factory* f) {

    if(nullptr == f) {
        f = factory();
    }

    std::set<Object> objList;
    const StatementList& statements = f->find();
    for(const Statement& stmt : statements) {
        objList.insert(Object(f->createResourceFromNode(stmt.subject)));
    }
    return objList;
}

std::set<Object> Object::findSources() const {
    std::set<Object> objList;
    Statement query;
    query.object = currentNode();
    const StatementList& statements = factory()->find(query);
    for (const Statement& stmt: statements) {
        Node rawSourceNode(stmt.subject);
        Resource rawSourceResource(factory()->createResourceFromNode(rawSourceNode));
        // Test for reified statement
        // In order to speed up sources lookup in reified environment, we don't check if the surrounding object
        // is really of type rdf:statement, we just assume it is
        Node subjectNode = factory()->findTarget(rawSourceNode, /* predicate */ Node().setIri(RDF_SUBJECT));
        if (!subjectNode.empty()) {
            auto subjectObject = Object(factory()->createResourceFromNode(subjectNode));
            if (subjectObject != *this) {
                objList.insert(subjectObject);
            }
        } else {
            objList.insert(Object(rawSourceResource));
        }
    }
    return objList;
}

std::set<Object> Object::findTargets() const {
    std::set<Object> objList;
    // Non reified statements
    Statement query;
    query.subject = currentNode();
    const StatementList& statements = factory()->find(query);
    for (const Statement& stmt: statements) {
        if ( stmt.object.type() == NodeType::RESOURCE || stmt.object.type() == NodeType::BLANK) {
            objList.insert(Object(factory()->createResourceFromNode(stmt.object)));
        }
    }
    // Reified statements
    NodeList objectReifiedStatements = reificationResourcesForCurrentObject();
    for (const Node& objectReifiedStatement : objectReifiedStatements ) {
        Resource reifiedStatement(factory()->createResourceFromNode(objectReifiedStatement));
        std::shared_ptr<Property> prop = reifiedStatement.getProperty(RDF_OBJECT);
        if ( prop->isResource() ) {
            objList.insert(prop->asResource());
        }
    }
    return objList;
}

std::set<Object> Object::findReified() const {
    const NodeList nodesReferingToThisObject = reificationResourcesForCurrentObject();

    // Iterate through statements and find ones matching predicate
    std::set<Object> allReifiedValues;
    for (const Node& thisObject : nodesReferingToThisObject ) {
        Resource reifiedStatement(factory()->createResourceFromNode(thisObject));
        std::shared_ptr<Property> predicate = reifiedStatement.getProperty(RDF_PREDICATE);
        if ( predicate ) {
            std::shared_ptr<Property> prop = reifiedStatement.getProperty(RDF_OBJECT);
            if ( prop->isResource() ) {
                allReifiedValues.insert(Object(prop->asResource()));
            }
        }
    }
    return allReifiedValues;
}

//TODO : add a version that also clone ressources
Object Object::cloneRecursiveStopAtResources(const Uri& newIri, bool(*doNotClone)(const Object &, const std::string &, const Object *)) const {
    return cloneRecursiveStopAtResourcesInternal(newIri, doNotClone, true);
}

Object Object::cloneRecursiveStopAtResourcesInternal(const Uri& newIri, bool(*doNotClone)(const Object &, const std::string &, const Object *), bool first) const {
    Object res(newIri, this->_rdfTypeIRI);
    copyPropertiesInternal(doNotClone, res, first);
    copyReifiedProperties(doNotClone, res);
    return res;
}

void Object::copyReifiedProperties(bool(*doNotClone)(const Object &, const std::string &, const Object *), Object& to) const {
    NodeList reifiedNodes = reificationResourcesForCurrentObject();
    for (const Node & node : reifiedNodes) {
        Resource reifiedStatement(factory()->createResourceFromNode(node));
        if (doNotClone && doNotClone(reifiedStatement, reifiedStatement.getProperty(RDF_PREDICATE)->iri(), this)) {
            continue;
        }
        std::shared_ptr<Property> predicate = reifiedStatement.getProperty(RDF_PREDICATE);
        std::shared_ptr<Property> object = reifiedStatement.getProperty(RDF_OBJECT);
        Object reifiedObject;
        switch (object->type()) {
            case NodeType::LITERAL:
                reifiedObject = to.reifyPropertyValue(predicate->value(), object->value());
                break;
            case NodeType::RESOURCE:
                reifiedObject = to.reifyObject(predicate->value(), Object(object->asResource()));
                break;
            case NodeType::BLANK:
                reifiedObject = to.reifyObject(predicate->value(),
                                               Object(object->asResource()).cloneRecursiveStopAtResourcesInternal("",
                                                                                                          doNotClone));
                break;
            default:
                throw InternalError("cloneRecursiveStopAtIri : reified object has invalid type");
        }
        Object(reifiedStatement).copyPropertiesInternal(doNotClone, reifiedObject);
    }
}

void Object::copyPropertiesInternal(bool(*doNotClone)(const Object &, const std::string &, const Object *), Object& to, bool first) const {
    if (first && doNotClone && doNotClone(_r, "", nullptr)) {
        return;
    }
    std::shared_ptr<std::list<Property>> properties = _r.getPropertyValues();
    // All of these are handled specially - do not copy
    static const std::vector<std::string> forbiddenProperties({RDF_OBJECT, RDF_SUBJECT, RDF_PREDICATE});
    for (const Property& property : *properties) {
        if (std::find(forbiddenProperties.begin(), forbiddenProperties.end(), property.iri()) != forbiddenProperties.end()) {
            continue;
        }
        if (property.isLiteral()) {
            to.addPropertyValue(property.iri(), property.value(), false);
        } else if (property.type() == NodeType::BLANK) {
            if (!doNotClone || !doNotClone(property.asResource(), property.iri(), this)) {
                to.addObject(property.iri(), Object(property.asResource()).cloneRecursiveStopAtResourcesInternal("", doNotClone), false);
            }
        } else {
            // Make only shallow copy of ressources
            to.addObject(property.iri(), property.asResource(), false);
        }
    }
}
}
