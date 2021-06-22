#include "autordf/ontology/Ontology.h"

#include "autordf/Factory.h"
#include "autordf/Object.h"
#include "autordf/ontology/RdfsEntity.h"

namespace autordf {

namespace ontology {

const std::string Ontology::RDF_NS = "http://www.w3.org/1999/02/22-rdf-syntax-ns#";
const std::string Ontology::RDFS_NS = "http://www.w3.org/2000/01/rdf-schema#";
const std::string Ontology::OWL_NS  = "http://www.w3.org/2002/07/owl#";
const std::string Ontology::AUTORDF_NS  = "http://github.com/ariadnext/AutoRDF#";

Ontology::Ontology(const Factory* f, bool verbose) : _verbose(verbose), _f(f) {
    populateSchemaClasses(f);
}

const Model* Ontology::model() const {
    return _f;
}

void Ontology::populateSchemaClasses(const Factory *f) {
    autordf::Object::setFactory(const_cast<Factory*>(f)) ;

    // BEGIN Add well known classes //
    auto owlThing = std::make_shared<Klass>(this);
    owlThing->_rdfname = OWL_NS + "Thing";
    owlThing->_label = {"This class is the ancestor for all user defined classes"};
    addClass(owlThing);

    auto rdfsResource = std::make_shared<Klass>(this);
    rdfsResource->_rdfname = RDFS_NS + "Resource";
    rdfsResource->_label = {"This class is provided for compatibility with RDFS ontologies."};
    rdfsResource->_directAncestors.insert(owlThing->_rdfname);
    addClass(rdfsResource);

    addHardcodedAnnotationProperty(OWL_NS  + "versionInfo", "Provides basic information for version control purpose");
    addHardcodedAnnotationProperty(RDFS_NS + "label",       "Supports a natural language label for the resource/property");
    addHardcodedAnnotationProperty(RDFS_NS + "comment",     "Supports a natural language comment about a resource/property");
    addHardcodedAnnotationProperty(RDFS_NS + "seeAlso",     "Provides a way to identify more information about the resource");
    addHardcodedAnnotationProperty(RDFS_NS + "isDefinedBy", "Provides a link pointing to the source of information about the resource");

    // END Add well known classes //

    // Gather annotation Properties
    const std::vector<Object>& owlAnnotationProperties = Object::findByType(OWL_NS + "AnnotationProperty");
    for ( auto const& owlAnnotationProperty : owlAnnotationProperties) {
        if ( _verbose ) {
            std::cout << "Found annotation property " << owlAnnotationProperty.iri() << std::endl;
        }
        auto p = std::make_shared<AnnotationProperty>(this);
        extractRDFS(owlAnnotationProperty, p.get());
        extractProperty(owlAnnotationProperty, p.get());
        addAnnotationProperty(p);
    }

    // Gather data Properties
    const std::vector<Object>& owlDataProperties = Object::findByType(OWL_NS + "DatatypeProperty");
    for ( auto const& owlDataProperty : owlDataProperties) {
        if ( _verbose ) {
            std::cout << "Found data property " << owlDataProperty.iri() << std::endl;
        }
        auto p = std::make_shared<DataProperty>(this);
        extractRDFS(owlDataProperty, p.get());
        extractProperty(owlDataProperty, p.get());
        addDataProperty(p);
    }

    // Gather object Properties
    const std::vector<Object>& owlObjectProperties = Object::findByType(OWL_NS + "ObjectProperty");
    for ( auto const& owlObjectProperty : owlObjectProperties) {
        if ( _verbose ) {
            std::cout << "Found object property " << owlObjectProperty.iri() << std::endl;
        }
        auto p = std::make_shared<ObjectProperty>(this);
        extractRDFS(owlObjectProperty, p.get());
        extractProperty(owlObjectProperty, p.get());
        addObjectProperty(p);
    }

    // Gather classes
    extractClasses(OWL_NS + "Class");
    extractClasses(RDFS_NS + "Class");

    // Remove reference to unexisting classes
    for ( auto const& klasses : _classUri2Ptr ) {
        std::set<std::string>& directAncestors = klasses.second->_directAncestors;
        for ( auto ancestor = directAncestors.begin(); ancestor != directAncestors.end(); ) {
            if ( _classUri2Ptr.find(*ancestor) == _classUri2Ptr.end() ) {
                std::cerr << "Class " << klasses.first << " has unreachable ancestor " << *ancestor << ", ignoring ancestor" << std::endl;
                ancestor = directAncestors.erase(ancestor);
            } else {
                ++ancestor;
            }
        }
    }

    // Make links between properties and classes
    for ( auto const& annotationPropertyMapItem : _annotationPropertyUri2Ptr ) {
        const AnnotationProperty& annotationProperty = *annotationPropertyMapItem.second;
        for ( const std::string& currentDomain : annotationProperty.domains() ) {
            auto klassIt = _classUri2Ptr.find(currentDomain);
            if ( klassIt != _classUri2Ptr.end() ) {
                klassIt->second->_annotationProperties.insert(annotationPropertyMapItem.second);
            } else  {
                std::cerr << "Property " << annotationProperty._rdfname << " refers to unreachable rdfs class " << currentDomain << ", skipping" << std::endl;
            }
        }
        if ( annotationProperty.domains().empty() ) {
            if ( _verbose ) {
                std::cout << annotationProperty.rdfname() << " has no domain, will attach to owl:Thing" << std::endl;
            }
            _classUri2Ptr[OWL_NS + "Thing"]->_annotationProperties.insert(annotationPropertyMapItem.second);
        }
    }
    for ( auto const& dataPropertyMapItem : _dataPropertyUri2Ptr ) {
        const DataProperty& dataProperty = *dataPropertyMapItem.second;
        for ( const std::string& currentDomain : dataProperty.domains() ) {
            auto klassIt = _classUri2Ptr.find(currentDomain);
            if ( klassIt != _classUri2Ptr.end() ) {
                klassIt->second->_dataProperties.insert(dataPropertyMapItem.second);
            } else  {
                std::cerr << "Property " << dataProperty._rdfname << " refers to unreachable rdfs class " << currentDomain << ", skipping" << std::endl;
            }
        }
        if ( dataProperty.domains().empty() ) {
            if ( _verbose ) {
                std::cout << dataProperty.rdfname() << " has no domain, will attach to owl:Thing" << std::endl;
            }
            _classUri2Ptr[OWL_NS + "Thing"]->_dataProperties.insert(dataPropertyMapItem.second);
        }
    }
    for ( auto const& objectPropertyMapItem : _objectPropertyUri2Ptr ) {
        const ObjectProperty& objectProperty = *objectPropertyMapItem.second;
        for ( const std::string& currentDomain : objectProperty.domains() ) {
            auto klassIt = _classUri2Ptr.find(currentDomain);
            if ( klassIt != _classUri2Ptr.end() ) {
                klassIt->second->_objectProperties.insert(objectPropertyMapItem.second);
            } else  {
                std::cerr << "Property " << objectProperty._rdfname << " refers to unreachable rdfs class " << currentDomain << ", skipping" << std::endl;
            }
        }
        if ( objectProperty.domains().empty() ) {
            if ( _verbose ) {
                std::cout << objectProperty.rdfname() << " has no domain, will attach to owl:Thing" << std::endl;
            }
            _classUri2Ptr[OWL_NS + "Thing"]->_objectProperties.insert(objectPropertyMapItem.second);
        }
    }

    // Checks keys do exist, and send them in proper place
    for ( auto & klsPair: _classUri2Ptr ) {
        auto kls = klsPair.second;
        for ( const std::string& key : kls->_keys ) {
            if ( _annotationPropertyUri2Ptr.count(key) ) {
                kls->_annotationKeys.insert( _annotationPropertyUri2Ptr[key]);
            } else if ( _objectPropertyUri2Ptr.count(key) ) {
                kls->_objectKeys.insert( _objectPropertyUri2Ptr[key]);
            } else if ( _dataPropertyUri2Ptr.count(key) ) {
                kls->_dataKeys.insert(_dataPropertyUri2Ptr[key]);
            } else {
                std::cerr << "Key " << key << " refers to unexisting Property, skipping" << std::endl;
            }
        }
    }
}

void Ontology::extractRDFS(const Object& o, RdfsEntity *rdfs) {
    rdfs->_rdfname = o.iri();
    rdfs->_comment = o.getPropertyValueList(RDFS_NS + "comment", false);
    rdfs->_label = o.getPropertyValueList(RDFS_NS + "label", false);
    rdfs->_seeAlso = o.getPropertyValueList(RDFS_NS + "seeAlso", false);
    rdfs->_isDefinedBy = o.getPropertyValueList(RDFS_NS + "isDefinedBy", false);
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
    const std::vector<Object>& subClasses = o.getObjectList(RDFS_NS + "subClassOf", false);
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
            if (containsObjectProperty(property.iri())) {
                kls->_objectProperties.insert(_objectPropertyUri2Ptr.at(property.iri()));
            } else {
                std::cerr << "Property " << property.iri() << " is referenced by anonymous class restriction, but is not defined anywhere, zapping." << std::endl;
            }
        } else {
            if (containsDataProperty(property.iri())) {
                kls->_dataProperties.insert(_dataPropertyUri2Ptr.at(property.iri()));
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
        if (o.getOptionalPropertyValue(OWL_NS + "onClass")) {
            kls->_overridenRange[property.iri()] = o.getPropertyValue(OWL_NS + "onClass");
        }
    }

    // Handle enum types
    std::shared_ptr<Object> oneof = o.getOptionalObject(OWL_NS + "oneOf");
    if ( oneof ) {
        std::shared_ptr<Object> rest = oneof;
        while ( rest && rest->iri() != RDF_NS + "nil" ) {
            Object oneOfObject(rest->getPropertyValue(RDF_NS + "first"));
            RdfsEntity oneOfVal(this);
            extractRDFS(oneOfObject, &oneOfVal);
            kls->_oneOfValues.insert(oneOfVal);
            rest = rest->getOptionalObject(RDF_NS + "rest");
        }
    }

    // Handle keys
    std::vector<Object> keys = o.getObjectList(OWL_NS + "hasKey", false);
    for ( const Object& key : keys ) {
        std::shared_ptr<Object> rest(new Object(key));
        while ( rest && rest->iri() != RDF_NS + "nil" ) {
            kls->_keys.insert(rest->getPropertyValue(RDF_NS + "first"));
            rest = rest->getOptionalObject(RDF_NS + "rest");
        }
    }

    // FIXME can loop endlessly
    const std::vector<Object>& equivalentClasses = o.getObjectList(OWL_NS + "equivalentClass", false);
    for ( const Object& equivalentClass: equivalentClasses ) {
        extractClass(equivalentClass, kls);
    }
}

void Ontology::extractProperty(const Object& o, Property *prop) {
    std::vector<Object> domainList = o.getObjectList(RDFS_NS + "domain", false);
    if ( domainList.size() == 1 ) {
        const Object& frontDomain = domainList.front();

        if ( frontDomain.iri().length() ) {
            prop->_domains.push_back(frontDomain.iri());
        } else {
            // Anonymous class, test for Union
            std::shared_ptr<Object> rest = frontDomain.getOptionalObject(OWL_NS + "unionOf");
            while ( rest && rest->iri() != RDF_NS + "nil" ) {
                Object unionOfObject(rest->getPropertyValue(RDF_NS + "first"));
                prop->_domains.push_back(unionOfObject.iri());
                rest = rest->getOptionalObject(RDF_NS + "rest");
            }
        }
    } else if ( domainList.size() > 1 ) {
        std::stringstream ss;
        std::cerr << "rdfs#domain has more than one item for " << o.iri() << ", skipping!" << std::endl;
    }

    std::vector<Object> rangeList = o.getObjectList(RDFS_NS + "range", false);
    if ( rangeList.size() == 1 ) {
        prop->_range = rangeList.front().iri();
    } else if ( rangeList.size() > 1 ) {
        std::stringstream ss;
        ss << "rdfs#range has more than one item for " << o.iri();
        throw std::runtime_error(ss.str());
    }
    if ( o.isA(OWL_NS + "FunctionalProperty") ) {
        prop->_minCardinality = 0;
        prop->_maxCardinality = 1;
    }

    prop->_ordered = o.getOptionalPropertyValue(AUTORDF_NS + "ordered") ? true : false;
}

void Ontology::extractClass(const Object& rdfsClass) {
    auto k = std::make_shared<Klass>(this);
    extractRDFS(rdfsClass, k.get());
    extractClass(rdfsClass, k.get());
    addClass(k);
}

void Ontology::extractClasses(const std::string& classTypeIRI) {
    const std::vector<Object>& classes = Object::findByType(classTypeIRI);
    for ( auto const& rdfsclass : classes) {
        if ( rdfsclass.iri().length() ) {
            if ( !containsClass(rdfsclass.iri()) ) {
                if ( !model()->iriPrefix(rdfsclass.iri()).empty() ) {
                    if ( _verbose ) {
                        std::cout << "Found class " << rdfsclass.iri() << std::endl;
                    }
                    auto k = std::make_shared<Klass>(this);
                    extractRDFS(rdfsclass, k.get());
                    extractClass(rdfsclass, k.get());
                    addClass(k);
                } else {
                    std::cerr << "No prefix found for class " << rdfsclass.iri() << " namespace, ignoring" << std::endl;
                }
            }
        }
    }
}

void Ontology::addHardcodedAnnotationProperty(const std::string& iri, const std::string& label) {
        const Object& object = Object(iri);
        auto p = std::make_shared<AnnotationProperty>(this);
        extractRDFS(object, p.get());
        extractProperty(object, p.get());
        if (p->label().empty()) {
            p->_label = {label};
        }
        if (p->domains().empty()) {
            p->_domains.push_back(OWL_NS + "Thing");
        }
        addAnnotationProperty(p);
}

void Ontology::addAnnotationProperty(const std::shared_ptr<AnnotationProperty>& obj) {
    if (_annotationPropertyUri2Ptr.find(obj->rdfname()) == _annotationPropertyUri2Ptr.end()) {
        _annotationPropertyUri2Ptr[obj->rdfname()] = obj;
    }
}
}
}
