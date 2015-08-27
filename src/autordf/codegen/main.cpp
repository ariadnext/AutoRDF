#include <getopt.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <set>
#include <map>
#include <memory>

#include <autordf/Factory.h>
#include <autordf/Object.h>

#include "DataProperty.h"
#include "ObjectProperty.h"
#include "Klass.h"
#include "Utils.h"

namespace autordf {
namespace codegen {

bool verbose = false;
bool generateAllInOne = false;

static const std::string RDF = "http://www.w3.org/1999/02/22-rdf-syntax-ns#";
static const std::string RDFS = "http://www.w3.org/2000/01/rdf-schema#";
static const std::string OWL  = "http://www.w3.org/2002/07/owl#";

void extractRDFS(const Object& o, RdfsEntity *rdfs) {
    rdfs->rdfname = o.iri();
    std::shared_ptr<PropertyValue> comment = o.getOptionalPropertyValue(RDFS + "comment");
    if (comment) {
        rdfs->comment = *comment;
    }
    std::shared_ptr<PropertyValue> label = o.getOptionalPropertyValue(RDFS + "label");
    if (label) {
        rdfs->label = *label;
    }
}

void extractClassCardinality(const Object& o, Klass *kls, const char * card, const char * minCard, const char * maxCard) {
    std::string propertyIRI = o.getPropertyValue(OWL + "onProperty");
    if ( o.getOptionalPropertyValue(OWL + card) ) {
        unsigned int cardinality = boost::lexical_cast<unsigned int>(o.getPropertyValue(OWL + card));
        kls->overridenMinCardinality[propertyIRI] = cardinality;
        kls->overridenMaxCardinality[propertyIRI] = cardinality;
    }
    if ( o.getOptionalPropertyValue(OWL + minCard) ) {
        kls->overridenMinCardinality[propertyIRI] = boost::lexical_cast<unsigned int>(o.getPropertyValue(OWL + minCard));
    }
    if ( o.getOptionalPropertyValue(OWL + maxCard) ) {
        kls->overridenMaxCardinality[propertyIRI] = boost::lexical_cast<unsigned int>(o.getPropertyValue(OWL + maxCard));
    }
}

void extractClass(const Object& o, Klass *kls) {
    const std::list<Object>& subClasses = o.getObjectList(RDFS + "subClassOf");
    for ( const Object& subclass : subClasses ) {
        if ( !subclass.iri().empty() ) {
            // This is a named ancestor, that will be processes seperately, handle that through
            // standard C++ inheritance mechanism
            kls->directAncestors.insert(subclass.iri());
        } else {
            // Anonymous ancestor, merge with current class
            extractClass(subclass, kls);
        }
    }
    kls->directAncestors.insert(OWL + "Thing");

    // If we are processing an anonymous ancestor
    if ( o.isA(OWL + "Restriction") ) {
        // Add class to list of known classes
        const Object& property = o.getObject(OWL + "onProperty");
        if (property.isA(OWL + "ObjectProperty")) {
            if (ObjectProperty::uri2Ptr.count(property.iri())) {
                kls->objectProperties.insert(ObjectProperty::uri2Ptr[property.iri()]);
            } else {
                std::cerr << "Property " << property.iri() << " is referenced by anonymous class restriction, but is not defined anywhere, zapping." << std::endl;
            }
        } else {
            if (DataProperty::uri2Ptr.count(property.iri())) {
                kls->dataProperties.insert(DataProperty::uri2Ptr[property.iri()]);
            } else {
                std::cerr << "Property " << property.iri() << " is referenced by anonymous class restriction, but is not defined anywhere, zapping." << std::endl;
            }
        }
        // FIXME: who has priority ?
        extractClassCardinality(o, kls, "cardinality", "minCardinality", "maxCardinality");
        extractClassCardinality(o, kls, "qualifiedCardinality", "minQualifiedCardinality", "maxQualifiedCardinality");

        if (o.getOptionalPropertyValue(OWL + "onDataRange")) {
            kls->overridenRange[property.iri()] = o.getPropertyValue(OWL + "onDataRange");
        }
    }

    // Handle enum types
    std::shared_ptr<Object> oneof = o.getOptionalObject(OWL + "oneOf");
    if ( oneof ) {
        std::shared_ptr<Object> rest = oneof;
        while ( rest && rest->iri() != RDF + "nil" ) {
            std::string enumIRI = rest->getPropertyValue(RDF + "first");
            kls->enumValues.insert(enumIRI);
            rest = rest->getOptionalObject(RDF + "rest");
        }
    }

    // FIXME can loop endlessly
    const std::list<Object>& equivalentClasses = o.getObjectList(OWL + "equivalentClass");
    for ( const Object& equivalentClass: equivalentClasses ) {
        extractClass(equivalentClass, kls);
    }
}

void extractProperty(const Object& o, Property *prop) {
    std::list<PropertyValue> domainList = o.getPropertyValueList(RDFS + "domain");
    for ( const PropertyValue& value: domainList ) {
        prop->domains.push_back(value);
    }
    std::list<PropertyValue> rangeList = o.getPropertyValueList(RDFS + "range");
    if ( rangeList.size() == 1 ) {
        prop->range = rangeList.front();
    } else if ( rangeList.size() > 1 ) {
        std::stringstream ss;
        ss << "rdfs#range has more than one item for " << o.iri();
        throw std::runtime_error(ss.str());
    }
    if ( o.isA(OWL + "FunctionalProperty") ) {
        prop->minCardinality = 0;
        prop->maxCardinality = 1;
    } else {
        prop->minCardinality = 0;
        prop->maxCardinality = 0xFFFFFFFF;
    }
}

void extractClass(const Object& rdfsClass) {
    auto k = std::make_shared<Klass>();
    extractRDFS(rdfsClass, k.get());
    extractClass(rdfsClass, k.get());
    Klass::uri2Ptr[k->rdfname] = k;
}

void extractClasses(const std::string& classTypeIRI) {
    const std::list<Object>& classes = Object::findByType(classTypeIRI);
    for ( auto const& rdfsclass : classes) {
        if ( rdfsclass.iri().length() ) {
            if ( Klass::uri2Ptr.find(rdfsclass.iri()) == Klass::uri2Ptr.end() ) {
                if ( !rdfPrefix(rdfsclass.iri(), Klass::model()).empty() ) {
                    if ( verbose ) {
                        std::cout << "Found class " << rdfsclass.iri() << std::endl;
                    }
                    auto k = std::make_shared<Klass>();
                    extractRDFS(rdfsclass, k.get());
                    extractClass(rdfsclass, k.get());
                    Klass::uri2Ptr[k->rdfname] = k;
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

void generateRdfTypeInfo() {
    std::ofstream oifs;
    createFile(Klass::outdir + "/RdfTypeInfo.h", &oifs);
    generateCodeProtectorBegin(oifs, "", "RdfTypeInfo");
    oifs << "class RdfTypeInfo {" << std::endl;
    oifs << "public:" << std::endl;
    indent(oifs, 1) << "RdfTypeInfo();" << std::endl;
    oifs << std::endl;
    indent(oifs, 1) << "static const std::map<std::string, std::set<std::string> >& data() { return DATA; }" << std::endl;
    oifs << "private:" << std::endl;
    indent(oifs, 1) << "static std::map<std::string, std::set<std::string> > DATA;" << std::endl;
    oifs << "};" << std::endl;
    oifs << std::endl;
    generateCodeProtectorEnd(oifs, "", "RdfTypeInfo");

    std::ofstream ofs;
    createFile(Klass::outdir + "/RdfTypeInfo.cpp", &ofs);
    addBoilerPlate(ofs);
    ofs << std::endl;
    ofs << "#include <map>" << std::endl;
    ofs << "#include <set>" << std::endl;
    ofs << "#include <string>" << std::endl;
    ofs << std::endl;
    ofs << "#include \"RdfTypeInfo.h\"" << std::endl;
    ofs << std::endl;
    for ( auto const& klassMapItem: Klass::uri2Ptr) {
        const Klass& cls = *klassMapItem.second;
        ofs << "#include \"" << cls.genCppNameSpace() << "/" << cls.genCppName() << ".h" << "\"" << std::endl;
    }
    ofs << std::endl;

    ofs << "std::map<std::string, std::set<std::string> > RdfTypeInfo::DATA;" << std::endl;
    ofs << std::endl;
    ofs << "RdfTypeInfo::RdfTypeInfo() {" << std::endl;
    indent(ofs, 1) << "if ( DATA.empty() ) {" << std::endl;
    for ( auto const& klassMapItem: Klass::uri2Ptr) {
        const Klass& cls = *klassMapItem.second;
        indent(ofs, 2) << "DATA[\"" << klassMapItem.first << "\"] = " << cls.genCppNameSpace() << "::" << cls.genCppName() << "::ancestorsRdfTypeIRI();" << std::endl;
    }
    indent(ofs, 1) << "};" << std::endl;
    ofs << std::endl;
    ofs << "}" << std::endl;
    ofs << std::endl;
    ofs << "namespace {" << std::endl;
    ofs << "RdfTypeInfo __loader;" << std::endl;
    ofs << "}" << std::endl;
}

void run() {
    // A well known classes:
    // FIXME add coments
    auto owlThing = std::make_shared<Klass>();
    owlThing->rdfname = OWL + "Thing";
    Klass::uri2Ptr[owlThing->rdfname] = owlThing;

    auto rdfsResource = std::make_shared<Klass>();
    rdfsResource->rdfname = RDFS + "Resource";
    rdfsResource->directAncestors.insert(owlThing->rdfname);
    Klass::uri2Ptr[rdfsResource->rdfname] = rdfsResource;

    // Gather data Properties
    const std::list<Object>& owlDataProperties = Object::findByType(OWL + "DatatypeProperty");
    for ( auto const& owlDataProperty : owlDataProperties) {
        if ( verbose ) {
            std::cout << "Found data property " << owlDataProperty.iri() << std::endl;
        }
        auto p = std::make_shared<DataProperty>();
        extractRDFS(owlDataProperty, p.get());
        extractProperty(owlDataProperty, p.get());
        DataProperty::uri2Ptr[p->rdfname] = p;
    }

    // Gather object Properties
    const std::list<Object>& owlObjectProperties = Object::findByType(OWL + "ObjectProperty");
    for ( auto const& owlObjectProperty : owlObjectProperties) {
        if ( verbose ) {
            std::cout << "Found object property " << owlObjectProperty.iri() << std::endl;
        }
        auto p = std::make_shared<ObjectProperty>();
        extractRDFS(owlObjectProperty, p.get());
        extractProperty(owlObjectProperty, p.get());
        ObjectProperty::uri2Ptr[p->rdfname] = p;
    }

    // Gather classes
    extractClasses(OWL + "Class");
    extractClasses(RDFS + "Class");

    // Remove reference to unexisting classes
    for ( auto const& klasses : Klass::uri2Ptr) {
        std::set<std::string>& directAncestors = klasses.second->directAncestors;
        for ( auto ancestor = directAncestors.begin(); ancestor != directAncestors.end(); ) {
            if ( Klass::uri2Ptr.find(*ancestor) == Klass::uri2Ptr.end() ) {
                std::cerr << "Class " << klasses.first << " has unreachable ancestor " << *ancestor << ", ignoring ancestor" << std::endl;
                ancestor = directAncestors.erase(ancestor);
            } else {
                ++ancestor;
            }
        }
    }

    // Make links between properties and classes
    for ( auto const& dataPropertyMapItem : DataProperty::uri2Ptr ) {
        const DataProperty& dataProperty = *dataPropertyMapItem.second;
        for(const std::string& currentDomain : dataProperty.domains) {
            auto klassIt = Klass::uri2Ptr.find(currentDomain);
            if ( klassIt != Klass::uri2Ptr.end() ) {
                klassIt->second->dataProperties.insert(dataPropertyMapItem.second);
            } else  {
                std::cerr << "Property " << dataProperty.rdfname << " refers to unreachable rdfs class " << currentDomain << ", skipping" << std::endl;
            }
        }
    }
    for ( auto const& objectPropertyMapItem : ObjectProperty::uri2Ptr ) {
        const ObjectProperty& objectProperty = *objectPropertyMapItem.second;
        for(const std::string& currentDomain : objectProperty.domains) {
            auto klassIt = Klass::uri2Ptr.find(currentDomain);
            if ( klassIt != Klass::uri2Ptr.end() ) {
                klassIt->second->objectProperties.insert(objectPropertyMapItem.second);
            } else  {
                std::cerr << "Property " << objectProperty.rdfname << " refers to unreachable rdfs class " << currentDomain << ", skipping" << std::endl;
            }
        }
    }

    // Starting code Generation
    std::set<std::string> cppNameSpaces;
    for ( auto const& klassMapItem: Klass::uri2Ptr) {
        // created directory if needed
        std::string cppNameSpace = klassMapItem.second->genCppNameSpace();
        cppNameSpaces.insert(cppNameSpace);
        createDirectory(Klass::outdir + "/" + cppNameSpace);

        klassMapItem.second->generateInterfaceDeclaration();
        klassMapItem.second->generateInterfaceDefinition();
        klassMapItem.second->generateDeclaration();
        klassMapItem.second->generateDefinition();
    }

    // Generate all TypesInfo
    generateRdfTypeInfo();

    //Generate all inclusions file
    for ( const std::string& cppNameSpace : cppNameSpaces ) {
        std::ofstream ofs;
        createFile(Klass::outdir + "/" + cppNameSpace + "/" + cppNameSpace + ".h", &ofs);

        generateCodeProtectorBegin(ofs, cppNameSpace, cppNameSpace);
        for ( auto const& klassMapItem: Klass::uri2Ptr) {
            if ( klassMapItem.second->genCppNameSpace() == cppNameSpace ) {
                const Klass& cls = *klassMapItem.second;
                ofs << "#include <" << cppNameSpace << "/" << cls.genCppName() << ".h" << ">" << std::endl;
            }
        }
        ofs << std::endl;
        generateCodeProtectorEnd(ofs, cppNameSpace, cppNameSpace);
    }

    // Generate all in one cpp file
    if ( generateAllInOne ) {
        std::ofstream ofs;
        createFile(Klass::outdir + "/AllInOne.cpp", &ofs);

        addBoilerPlate(ofs);
        ofs << std::endl;
        ofs << "#include \"RdfTypeInfo.cpp\"" << std::endl;
        for ( auto const& klassMapItem: Klass::uri2Ptr) {
            const Klass& cls = *klassMapItem.second;
            ofs << "#include \"" << cls.genCppNameSpace() << "/I" << cls.genCppName() << ".cpp" << "\"" << std::endl;
            ofs << "#include \"" << cls.genCppNameSpace() << "/" << cls.genCppName() << ".cpp" << "\"" << std::endl;
        }
        ofs << std::endl;
    }
}

}
}

int main(int argc, char **argv) {
    autordf::Factory f;

    std::stringstream usage;
    usage << "Usage: " << argv[0] << " [-v] [-a] [-n namespacemap] [-o outdir] owlfile1 [owlfile2...]\n";
    usage << "\t" << "Processes an OWL file, and generates C++ classes from it in current directory\n";
    usage << "\t" << "namespacemap:\t Adds supplementary namespaces prefix definition, in the form 'prefix:namespace IRI'. Defaults to empty.\n";
    usage << "\t" << "outdir:\t Folder where to generate files in. If it does not exit it will be created. Defaults to current directory." << ".\n";
    usage << "\t" << "-v:\t Turn verbose output on." << ".\n";
    usage << "\t" << "-a:\t Generate one cpp file that includes all the other called AllInOne.cpp" << ".\n";
    int opt;
    while ((opt = ::getopt(argc, argv, "avhn:o:")) != -1) {
        switch (opt) {
            case 'n': {
                {
                    std::stringstream ss(optarg);
                    std::string prefix;
                    std::getline(ss, prefix, ':');
                    std::string ns;
                    ss >> ns;
                    if ( autordf::codegen::verbose ) {
                        std::cout << "Adding  " << prefix << "-->" << ns << " map." << std::endl;
                    }
                    f.addNamespacePrefix(prefix, ns);
                }
                break;
            }
            case 'o':
                autordf::codegen::Klass::outdir = optarg;
                break;
            case 'v':
                autordf::codegen::verbose = true;
                break;
            case 'a':
                autordf::codegen::generateAllInOne = true;
                break;
            case 'h':
            default: /* '?' */
                std::cerr << usage.str();
                std::cerr.flush();
                return 1;
        }
    }


    if (optind >= argc) {
        std::cerr << "Expected argument after options" << std::endl;
        return 1;
    }

    autordf::codegen::createDirectory(autordf::codegen::Klass::outdir);

    // Hardcode some prefixes
    f.addNamespacePrefix("owl", autordf::codegen::OWL);
    f.addNamespacePrefix("rdfs", autordf::codegen::RDFS);
    //FIXME: Read that from command line
    std::string baseURI = "http://";

    autordf::codegen::RdfsEntity::setModel(&f);
    autordf::Object::setFactory(&f);
    while ( optind < argc ) {
        if ( autordf::codegen::verbose ) {
            std::cout << "Loading " << argv[optind] << " into model." << std::endl;
        }
        f.loadFromFile(argv[optind], baseURI);
        optind++;
    }
    autordf::codegen::run();
    return 0;
}
