#include "autordf/ontology/Ontology.h"

#include "autordf/Factory.h"
#include "autordf/Object.h"
#include "autordf/ontology/RdfsEntity.h"
#include "autordf/ontology/Klass.h"

namespace autordf {

// Checks an returns if available registered prefix for IRI
std::string rdfPrefix(const std::string& rdfiri, const Model *model) {
    for ( const std::pair<std::string, std::string>& prefixMapItem : model->namespacesPrefixes() ) {
        const std::string& iri = prefixMapItem.second;
        if ( rdfiri.substr(0, iri.length()) == iri ) {
            return prefixMapItem.first;
        }
    }
    return "";
}

namespace ontology {

const std::string Ontology::RDF_NS = "http://www.w3.org/1999/02/22-rdf-syntax-ns#";
const std::string Ontology::RDFS_NS = "http://www.w3.org/2000/01/rdf-schema#";
const std::string Ontology::OWL_NS  = "http://www.w3.org/2002/07/owl#";
bool Ontology::_verbose = false;

void Ontology::populateSchemaClasses(Factory *f, bool verbose) {
    _verbose = verbose;
    autordf::ontology::RdfsEntity::setModel(f);
    autordf::Object::setFactory(f);

    // A well known classes:
    // FIXME add coments
    auto owlThing = std::make_shared<Klass>();
    owlThing->_rdfname = OWL_NS + "Thing";
    Klass::add(owlThing);

    auto rdfsResource = std::make_shared<Klass>();
    rdfsResource->_rdfname = RDFS_NS + "Resource";
    rdfsResource->_directAncestors.insert(owlThing->_rdfname);
    Klass::add(rdfsResource);

    // Gather data Properties
    const std::list<Object>& owlDataProperties = Object::findByType(OWL_NS + "DatatypeProperty");
    for ( auto const& owlDataProperty : owlDataProperties) {
        if ( _verbose ) {
            std::cout << "Found data property " << owlDataProperty.iri() << std::endl;
        }
        auto p = std::make_shared<DataProperty>();
        extractRDFS(owlDataProperty, p.get());
        extractProperty(owlDataProperty, p.get());
        DataProperty::add(p);
    }

    // Gather object Properties
    const std::list<Object>& owlObjectProperties = Object::findByType(OWL_NS + "ObjectProperty");
    for ( auto const& owlObjectProperty : owlObjectProperties) {
        if ( verbose ) {
            std::cout << "Found object property " << owlObjectProperty.iri() << std::endl;
        }
        auto p = std::make_shared<ObjectProperty>();
        extractRDFS(owlObjectProperty, p.get());
        extractProperty(owlObjectProperty, p.get());
        ObjectProperty::add(p);
    }

    // Gather classes
    extractClasses(OWL_NS + "Class");
    extractClasses(RDFS_NS + "Class");

    // Remove reference to unexisting classes
    for ( auto const& klasses : Klass::uri2Ptr() ) {
        std::set<std::string>& directAncestors = klasses.second->_directAncestors;
        for ( auto ancestor = directAncestors.begin(); ancestor != directAncestors.end(); ) {
            if ( Klass::uri2Ptr().find(*ancestor) == Klass::uri2Ptr().end() ) {
                std::cerr << "Class " << klasses.first << " has unreachable ancestor " << *ancestor << ", ignoring ancestor" << std::endl;
                ancestor = directAncestors.erase(ancestor);
            } else {
                ++ancestor;
            }
        }
    }

    // Make links between properties and classes
    for ( auto const& dataPropertyMapItem : DataProperty::uri2Ptr() ) {
        const DataProperty& dataProperty = *dataPropertyMapItem.second;
        for(const std::string& currentDomain : dataProperty.domains()) {
            auto klassIt = Klass::uri2Ptr().find(currentDomain);
            if ( klassIt != Klass::uri2Ptr().end() ) {
                klassIt->second->_dataProperties.insert(dataPropertyMapItem.second);
            } else  {
                std::cerr << "Property " << dataProperty._rdfname << " refers to unreachable rdfs class " << currentDomain << ", skipping" << std::endl;
            }
        }
    }
    for ( auto const& objectPropertyMapItem : ObjectProperty::uri2Ptr() ) {
        const ObjectProperty& objectProperty = *objectPropertyMapItem.second;
        for(const std::string& currentDomain : objectProperty.domains()) {
            auto klassIt = Klass::uri2Ptr().find(currentDomain);
            if ( klassIt != Klass::uri2Ptr().end() ) {
                klassIt->second->_objectProperties.insert(objectPropertyMapItem.second);
            } else  {
                std::cerr << "Property " << objectProperty._rdfname << " refers to unreachable rdfs class " << currentDomain << ", skipping" << std::endl;
            }
        }
    }
}

void Ontology::extractRDFS(const Object& o, RdfsEntity *rdfs) {
    rdfs->_rdfname = o.iri();
    std::shared_ptr<PropertyValue> comment = o.getOptionalPropertyValue(RDFS_NS + "comment");
    if (comment) {
        rdfs->_comment = *comment;
    }
    std::shared_ptr<PropertyValue> label = o.getOptionalPropertyValue(RDFS_NS + "label");
    if (label) {
        rdfs->_label = *label;
    }
}

void Ontology::extractClassCardinality(const Object& o, Klass *kls, const char * card, const char * minCard, const char * maxCard) {
    std::string propertyIRI = o.getPropertyValue(OWL_NS + "onProperty");
    if ( o.getOptionalPropertyValue(OWL_NS + card) ) {
        unsigned int cardinality = boost::lexical_cast<unsigned int>(o.getPropertyValue(OWL_NS + card));
        kls->_overridenMinCardinality[propertyIRI] = cardinality;
        kls->_overridenMaxCardinality[propertyIRI] = cardinality;
    }
    if ( o.getOptionalPropertyValue(OWL_NS + minCard) ) {
        kls->_overridenMinCardinality[propertyIRI] = boost::lexical_cast<unsigned int>(o.getPropertyValue(OWL_NS + minCard));
    }
    if ( o.getOptionalPropertyValue(OWL_NS + maxCard) ) {
        kls->_overridenMaxCardinality[propertyIRI] = boost::lexical_cast<unsigned int>(o.getPropertyValue(OWL_NS + maxCard));
    }
}

void Ontology::extractClass(const Object& o, Klass *kls) {
    const std::list<Object>& subClasses = o.getObjectList(RDFS_NS + "subClassOf");
    for ( const Object& subclass : subClasses ) {
        if ( !subclass.iri().empty() ) {
            // This is a named ancestor, that will be processes seperately, handle that through
            // standard C++ inheritance mechanism
            kls->_directAncestors.insert(subclass.iri());
        } else {
            // Anonymous ancestor, merge with current class
            extractClass(subclass, kls);
        }
    }
    kls->_directAncestors.insert(OWL_NS + "Thing");

    // If we are processing an anonymous ancestor
    if ( o.isA(OWL_NS + "Restriction") ) {
        // Add class to list of known classes
        const Object& property = o.getObject(OWL_NS + "onProperty");
        if (property.isA(OWL_NS + "ObjectProperty")) {
            if (ObjectProperty::contains(property.iri())) {
                kls->_objectProperties.insert(ObjectProperty::uri2Ptr().at(property.iri()));
            } else {
                std::cerr << "Property " << property.iri() << " is referenced by anonymous class restriction, but is not defined anywhere, zapping." << std::endl;
            }
        } else {
            if (DataProperty::contains(property.iri())) {
                kls->_dataProperties.insert(DataProperty::uri2Ptr().at(property.iri()));
            } else {
                std::cerr << "Property " << property.iri() << " is referenced by anonymous class restriction, but is not defined anywhere, zapping." << std::endl;
            }
        }
        // FIXME: who has priority ?
        extractClassCardinality(o, kls, "cardinality", "minCardinality", "maxCardinality");
        extractClassCardinality(o, kls, "qualifiedCardinality", "minQualifiedCardinality", "maxQualifiedCardinality");

        if (o.getOptionalPropertyValue(OWL_NS + "onDataRange")) {
            kls->_overridenRange[property.iri()] = o.getPropertyValue(OWL_NS + "onDataRange");
        }
    }

    // Handle enum types
    std::shared_ptr<Object> oneof = o.getOptionalObject(OWL_NS + "oneOf");
    if ( oneof ) {
        std::shared_ptr<Object> rest = oneof;
        while ( rest && rest->iri() != RDF_NS + "nil" ) {
            Object oneOfObject(rest->getPropertyValue(RDF_NS + "first"));
            RdfsEntity oneOfVal;
            extractRDFS(oneOfObject, &oneOfVal);
            kls->_oneOfValues.insert(oneOfVal);
            rest = rest->getOptionalObject(RDF_NS + "rest");
        }
    }

    // FIXME can loop endlessly
    const std::list<Object>& equivalentClasses = o.getObjectList(OWL_NS + "equivalentClass");
    for ( const Object& equivalentClass: equivalentClasses ) {
        extractClass(equivalentClass, kls);
    }
}

void Ontology::extractProperty(const Object& o, Property *prop) {
    std::list<PropertyValue> domainList = o.getPropertyValueList(RDFS_NS + "domain");
    for ( const PropertyValue& value: domainList ) {
        prop->_domains.push_back(value);
    }
    std::list<PropertyValue> rangeList = o.getPropertyValueList(RDFS_NS + "range");
    if ( rangeList.size() == 1 ) {
        prop->_range = rangeList.front();
    } else if ( rangeList.size() > 1 ) {
        std::stringstream ss;
        ss << "rdfs#range has more than one item for " << o.iri();
        throw std::runtime_error(ss.str());
    }
    if ( o.isA(OWL_NS + "FunctionalProperty") ) {
        prop->_minCardinality = 0;
        prop->_maxCardinality = 1;
    } else {
        prop->_minCardinality = 0;
        prop->_maxCardinality = 0xFFFFFFFF;
    }
}

void Ontology::extractClass(const Object& rdfsClass) {
    auto k = std::make_shared<Klass>();
    extractRDFS(rdfsClass, k.get());
    extractClass(rdfsClass, k.get());
    Klass::add(k);
}

void Ontology::extractClasses(const std::string& classTypeIRI) {
    const std::list<Object>& classes = Object::findByType(classTypeIRI);
    for ( auto const& rdfsclass : classes) {
        if ( rdfsclass.iri().length() ) {
            if ( !Klass::contains(rdfsclass.iri()) ) {
                if ( !rdfPrefix(rdfsclass.iri(), Klass::model()).empty() ) {
                    if ( _verbose ) {
                        std::cout << "Found class " << rdfsclass.iri() << std::endl;
                    }
                    auto k = std::make_shared<Klass>();
                    extractRDFS(rdfsclass, k.get());
                    extractClass(rdfsclass, k.get());
                    Klass::add(k);
                } else {
                    std::cerr << "No prefix found for class " << rdfsclass.iri() << " namespace, ignoring" << std::endl;
                }
            }
        } else {
            std::cerr << "Skipping blank node class" << std::endl;
            // anonymous class found
        }
    }
}

}
}
