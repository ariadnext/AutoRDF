#include <autordf/Object.h>

#include <autordf/Factory.h>
#include <autordf/Exception.h>

namespace autordf {

Factory *Object::_factory = nullptr;

const std::string Object::RDF_NS = "http://www.w3.org/1999/02/22-rdf-syntax-ns#";

void Object::setFactory(Factory *f) {
    _factory = f;
}

Object::Object(const std::string &iri, const std::string& rdfTypeIRI) : _r(iri.empty() ? _factory->createBlankNodeResource() :_factory->createIRIResource(iri)) {
    if ( !rdfTypeIRI.empty() ) {
        _rdfTypeIRI = rdfTypeIRI;
        _rdfTypeWritingRequired = true;
    } else {
        _rdfTypeWritingRequired = false;
    }
}

Object::Object(const Object& other) : _r(other._r), _rdfTypeWritingRequired(other._rdfTypeWritingRequired), _rdfTypeIRI(other._rdfTypeIRI) {
}

Object::Object(Resource r, const std::string& rdfTypeIRI) : _r(r) {
    if ( !rdfTypeIRI.empty() ) {
        _rdfTypeIRI = rdfTypeIRI;
        _rdfTypeWritingRequired = true;
    } else {
        _rdfTypeWritingRequired = false;
    }
}

const std::string& Object::iri() const {
    if ( _r.type() == NodeType::RESOURCE ) {
        return _r.name();
    } else {
        static const std::string& EMPTY="";
        return EMPTY;
    }
}

Object Object::getObject(const std::string &propertyIRI) const {
    return Object(_r.getProperty(propertyIRI).asResource());
}

std::shared_ptr<Object> Object::getOptionalObject(const std::string &propertyIRI) const {
    std::shared_ptr<Property> p(_r.getOptionalProperty(propertyIRI));
    if ( p ) {
        return std::shared_ptr<Object>(new Object(p->asResource()));
    } else {
        return nullptr;
    }
}

std::list<Object> Object::getObjectList(const std::string &propertyIRI) const {
    return getObjectListImpl<Object>(propertyIRI);
}

void Object::setObject(const std::string &propertyIRI, const Object &obj) {
    addRdfTypeIfNeeded();
    Property p = _factory->createProperty(propertyIRI);
    p.setValue(obj._r);
    _r.removeProperties(propertyIRI);
    _r.addProperty(p);
}

void Object::setObjectList(const std::string &propertyIRI, const std::list<Object> &values) {
    addRdfTypeIfNeeded();
    Property p =_factory->createProperty(propertyIRI);
    _r.removeProperties(propertyIRI);
    for (const Object& obj: values) {
        p.setValue(obj._r);
        _r.addProperty(p);
    }
}

PropertyValue Object::getPropertyValue(const std::string& propertyIRI) const {
    return _r.getProperty(propertyIRI).value();
}

std::shared_ptr<PropertyValue> Object::getOptionalPropertyValue(const std::string& propertyIRI) const {
    std::shared_ptr<Property> p(_r.getOptionalProperty(propertyIRI));
    if ( p ) {
        return std::make_shared<PropertyValue>(p->value());
    } else {
        return nullptr;
    }
}

std::list<PropertyValue> Object::getPropertyValueList(const std::string& propertyIRI) const {
    if ( propertyIRI.empty() ) {
        throw InvalidIRI("Calling Object::getPropertyValueList() with empty IRI is forbidden");
    }
    std::list<PropertyValue> valuesList;
    const std::list<Property>& propList = _r.getPropertyValues(propertyIRI);
    for (const Property& prop: propList) {
        valuesList.push_back(prop.value());
    }
    return valuesList;
}

void Object::setPropertyValue(const std::string& propertyIRI, const PropertyValue& val) {
    addRdfTypeIfNeeded();
    Property p = _factory->createProperty(propertyIRI);
    p.setValue(val);
    _r.removeProperties(propertyIRI);
    _r.addProperty(p);
}

void Object::addPropertyValue(const std::string& propertyIRI, const PropertyValue& val) {
    addRdfTypeIfNeeded();
    _r.addProperty(_factory->createProperty(propertyIRI).setValue(val));
}

void Object::setPropertyValueList(const std::string& propertyIRI, const std::list<PropertyValue>& values) {
    addRdfTypeIfNeeded();
    Property p = _factory->createProperty(propertyIRI);
    _r.removeProperties(propertyIRI);
    for (const PropertyValue& val: values) {
        p.setValue(val);
        _r.addProperty(p);
    }
}

bool Object::isA(const std::string& typeIRI) const {
    auto const& typesList = getPropertyValueList(RDF_NS + "type");
    for ( const PropertyValue& type: typesList) {
        if ( type == typeIRI ) {
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

Object Object::clone(const std::string& iri) {
    addRdfTypeIfNeeded();
    const std::list<Property>& props = _r.getPropertyValues("");
    if ( !iri.empty() ) {
        return Object(_factory->createIRIResource(iri).setProperties(props), _rdfTypeIRI);
    } else {
        return Object(_factory->createBlankNodeResource("").setProperties(props), _rdfTypeIRI);
    }
}

std::list<Object> Object::findByType(const std::string& iri) {
    return findHelper<Object>(iri);
}

void Object::addRdfTypeIfNeeded() {
    if ( _rdfTypeWritingRequired ) {
        _rdfTypeWritingRequired = false;
        addPropertyValue(RDF_NS + "type", _rdfTypeIRI);
    }
}

/**
 * Checks if current object is of rdf type that is either _rdfTypeIRI, or one of its subclasses
 * @param rdfTypesInfo, the types inferred for current class hierarchy
 * @param expectedTypeIRI
 */
void Object::runtimeTypeCheck(const std::map<std::string, std::set<std::string> >& rdfTypesInfo) const {
    const std::list<PropertyValue>& typesList = getPropertyValueList(RDF_NS + "type");
    if ( typesList.empty() ) {
        // No type, let's say that's ok
        return;
    }
    for ( const PropertyValue& t : typesList ) {
        if ( _rdfTypeIRI == t ) {
            return;
        }
        auto typeInfo = rdfTypesInfo.find(t);
        if ( typeInfo != rdfTypesInfo.end() ) {
            if ( typeInfo->second.count(_rdfTypeIRI) ) {
                return;
            }
        }
    }

    throw InvalidClass("Resource " + iri() + " is not of type " + _rdfTypeIRI + ", or one of its subclasses");
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
    os << "\"name\": \"" << _r.name() <<'"';
    std::set<std::string> donePropsIRI;
    // Get all props
    const std::list<Property>& propsList = _r.getPropertyValues("");
    for ( auto propit = propsList.begin(); propit != propsList.end(); ++propit ) {
        if ( !donePropsIRI.count(propit->iri()) ) {
            if ( propit != propsList.begin() ) {
                os << ',';
            }
            newLine(os, indentLevel);
            donePropsIRI.insert(propit->iri());
            os << '"' << propit->iri() << "\": ";
            if ( (recurse > 0) && (propit->type() == NodeType::BLANK || propit->type() == NodeType::RESOURCE ) && propit->asResource().getPropertyValues("").size() ) {
                Object child(propit->asResource());
                os << '{' << std::endl;
                child.printStream(os, recurse - 1, indentLevel + 1);
                newLine(os, indentLevel);
                os << '}';
            } else {
                const std::list<PropertyValue>& values = getPropertyValueList(propit->iri());
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
