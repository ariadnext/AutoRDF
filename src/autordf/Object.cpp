#include <autordf/Object.h>

#include <autordf/Factory.h>
#include <autordf/Exception.h>

namespace autordf {

Factory *Object::_factory = nullptr;

const std::string Object::RDF_NS = "http://www.w3.org/1999/02/22-rdf-syntax-ns#";

void Object::setFactory(Factory *f) {
    _factory = f;
}

Object::Object(const Uri &iri, const Uri& rdfTypeIRI) : _r(iri.empty() ? factory()->createBlankNodeResource() :factory()->createIRIResource(iri)) {
    construct(rdfTypeIRI);
}

Object::Object(const Object& other, const Uri& rdfTypeIRI) : _r(other._r) {
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

std::string Object::prefixedIri() const {
    std::pair<std::string, std::string> bestPrefix;

    Uri i = iri();
    for ( auto const& pair : factory()->namespacesPrefixes() ) {
        if ( (i.find(pair.second) == 0) && (i.length() > pair.second.length() ) ) {
            // Found !
            bestPrefix = pair;
        }
    }
    if ( !bestPrefix.second.empty() ) {
        return bestPrefix.first + ":" + i.substr(bestPrefix.second.length() + 1);
    } else {
        return i;
    }
}

std::vector<Uri> Object::getTypes(const std::string& namespaceFilter) const {
    std::vector<Uri> obj;
    auto types = getObjectList(autordf::Object::RDF_NS + "type");
    for (const autordf::Object& type: types) {
        if (namespaceFilter.empty() || type.iri().find(namespaceFilter) == 0) {
            obj.push_back(type.iri());
        }
    }
    return obj;
}

Object Object::getObject(const Uri &propertyIRI) const {
    return Object(_r.getProperty(propertyIRI)->asResource());
}

std::shared_ptr<Object> Object::getOptionalObject(const Uri& propertyIRI) const {
    std::shared_ptr<Property> p(_r.getOptionalProperty(propertyIRI));
    if ( p ) {
        return std::shared_ptr<Object>(new Object(p->asResource()));
    } else {
        return nullptr;
    }
}

std::vector<Object> Object::getObjectList(const Uri& propertyIRI) const {
    return getObjectListImpl<Object>(propertyIRI);
}

void Object::setObject(const Uri& propertyIRI, const Object& obj) {
    writeRdfType();
    std::shared_ptr<Property> p = factory()->createProperty(propertyIRI);
    p->setValue(obj._r);
    _r.removeProperties(propertyIRI);
    _r.addProperty(*p);
}

void Object::addObject(const Uri& propertyIRI, const Object& obj) {
    writeRdfType();
    std::shared_ptr<Property> p = factory()->createProperty(propertyIRI);
    p->setValue(obj._r);
    _r.addProperty(*p);
}

void Object::setObjectList(const Uri& propertyIRI, const std::vector<Object> &values) {
    setObjectListImpl(propertyIRI, values);
}

void Object::removeObject(const Uri& propertyIRI, const Object& obj) {
    std::shared_ptr<Property> p = factory()->createProperty(propertyIRI);
    p->setValue(obj._r);
    _r.removeSingleProperty(*p);
}

PropertyValue Object::getPropertyValue(const Uri& propertyIRI) const {
    return _r.getProperty(propertyIRI)->value();
}

std::shared_ptr<PropertyValue> Object::getOptionalPropertyValue(const Uri& propertyIRI) const {
    std::shared_ptr<Property> p(_r.getOptionalProperty(propertyIRI));
    if ( p ) {
        return std::make_shared<PropertyValue>(p->value());
    } else {
        return nullptr;
    }
}

PropertyValueVector Object::getPropertyValueList(const Uri& propertyIRI) const {
    if ( propertyIRI.empty() ) {
        throw InvalidIRI("Calling Object::getPropertyValueVector() with empty IRI is forbidden");
    }
    PropertyValueVector valuesList;
    const std::list<Property>& propList = _r.getPropertyValues(propertyIRI);
    for (const Property& prop: propList) {
        if (prop.isLiteral()) {
            valuesList.push_back(prop.value());
        }
    }
    return valuesList;
}

void Object::setPropertyValue(const Uri& propertyIRI, const PropertyValue& val) {
    writeRdfType();
    std::shared_ptr<Property> p = factory()->createProperty(propertyIRI);
    p->setValue(val);
    _r.removeProperties(propertyIRI);
    _r.addProperty(*p);
}

void Object::addPropertyValue(const Uri& propertyIRI, const PropertyValue& val) {
    writeRdfType();
    _r.addProperty(factory()->createProperty(propertyIRI)->setValue(val));
}

void Object::removePropertyValue(const Uri& propertyIRI, const PropertyValue& val) {
    std::shared_ptr<Property> p = factory()->createProperty(propertyIRI);
    p->setValue(val);
    _r.removeSingleProperty(*p);
}

void Object::setPropertyValueList(const Uri& propertyIRI, const PropertyValueVector& values) {
    writeRdfType();
    std::shared_ptr<Property> p = factory()->createProperty(propertyIRI);
    _r.removeProperties(propertyIRI);
    for (const PropertyValue& val: values) {
        p->setValue(val);
        _r.addProperty(*p);
    }
}

bool Object::isA(const Uri& typeIRI) const {
    auto const& typesList = getObjectList(RDF_NS + "type");
    for ( const Object& type: typesList) {
        if ( type.iri() == typeIRI ) {
            return true;
        }
    }
    return false;
}

void Object::remove() {
    _r.removeProperties("");
    if ( !_rdfTypeIRI.empty() ) {
        _rdfTypeWritingRequired = true;
    }
}

Object Object::clone(const Uri& iri) const {
    const std::list<Property>& props = _r.getPropertyValues();
    if ( !iri.empty() ) {
        return Object(factory()->createIRIResource(iri).setProperties(props), _rdfTypeIRI);
    } else {
        return Object(factory()->createBlankNodeResource("").setProperties(props), _rdfTypeIRI);
    }
}

std::vector<Object> Object::findByType(const Uri& iri) {
    return findHelper<Object>(iri);
}

Object Object::findByKey(const Uri& propertyIRI, const PropertyValue& value) {
    Statement query;
    query.predicate.setIri(propertyIRI);
    query.object.setLiteral(value, value.lang(), value.dataTypeIri());
    const StatementList& statements = factory()->find(query);
    if ( statements.size() == 0 ) {
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
    if ( statements.size() == 0 ) {
        throw ObjectNotFound(std::string("No object with owl key ") + propertyIRI + " set to " + object.iri() + " found");
    }
    if ( statements.size() > 1 ) {
        throw DuplicateObject(std::string("More than one object with owl key ") + propertyIRI + " set to " + object.iri() + " found");
    }
    return Object(factory()->createResourceFromNode(statements.begin()->subject));
}

void Object::writeRdfType() {
    if ( _rdfTypeWritingRequired ) {
        _rdfTypeWritingRequired = false;
        setObject(RDF_NS + "type", Object(_rdfTypeIRI));
    }
}

Factory *Object::factory() {
    if ( !_factory ) {
        throw std::runtime_error("You must call autordf::Object::setFactory() before using any of your objects methods");
    }
    return _factory;
}


namespace {

void indentLine(std::ostream& os, int indent) {
    for (unsigned int i = 0; i < indent; ++i) {
        os << '\t';
    }
}

void newLine(std::ostream& os, int indent) {
    os << std::endl;
    indentLine(os, indent);
}

}

std::ostream& operator<<(std::ostream& os, const Object& obj) {
    obj.printStream(os);
    return os;
}

std::ostream& Object::printStream(std::ostream& os, int recurse, int indentLevel) const {
    indentLine(os, indentLevel);
    os << "\"Subject\": \"" << _r.name() <<'"';
    std::set<std::string> donePropsIRI;
    // Get all props
    const std::list<Property>& propsList = _r.getPropertyValues();
    for ( auto propit = propsList.begin(); propit != propsList.end(); ++propit ) {
        if ( !donePropsIRI.count(propit->iri()) ) {
            if ( propit != propsList.begin() ) {
                os << ',';
            }
            newLine(os, indentLevel);
            donePropsIRI.insert(propit->iri());
            os << '"' << propit->iri() << "\": ";
            if ( (recurse > 0) && (propit->type() == NodeType::BLANK || propit->type() == NodeType::RESOURCE ) && propit->asResource().getPropertyValues().size() ) {
                Object child(propit->asResource());
                os << '{' << std::endl;
                child.printStream(os, recurse - 1, indentLevel + 1);
                newLine(os, indentLevel);
                os << '}';
            } else {
                const PropertyValueVector& values = getPropertyValueList(propit->iri());
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

}
