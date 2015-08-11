#include <sys/stat.h>
#include <string.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <memory>
#include <stdexcept>
#include <algorithm>

#include <autordf/Object.h>
#include <autordf/Factory.h>

namespace autordf {
namespace tools {

static const std::string RDFS = "http://www.w3.org/2000/01/rdf-schema";
static const std::string OWL  = "http://www.w3.org/2002/07/owl";

class ObjectProperty;
class DataProperty;
class klass;

void generateCode(const klass& kls, const std::string& cppNameSpace);
void generateCodeProptectorBegin(std::ofstream& ofs, const std::string& cppNameSpace, const std::string cppName);
void generateCodeProptectorEnd(std::ofstream& ofs, const std::string& cppNameSpace, const std::string cppName);

class RDFSEntity {
public:
    // Object iri
    std::string rdfname;
    std::string comment;
    std::string label;

    std::string genCppName() const {
        std::string cppname = rdfname.substr(rdfname.find_last_of("/") + 1);
        return cppname;
    }
};

class klass : public RDFSEntity {
public:
    std::list<std::shared_ptr<DataProperty> > dataProperties;
    std::list<std::shared_ptr<ObjectProperty> > objectProperties;

    // iri to klass map
    static std::map<std::string, std::shared_ptr<klass> > uri2Ptr;
};
std::map<std::string, std::shared_ptr<klass> > klass::uri2Ptr;

class Property : public RDFSEntity {
public:
    std::list<std::string> domains;
    std::string range;
};

class DataProperty : public Property {
public:
    // iri to Property map
    static std::map<std::string, std::shared_ptr<DataProperty> > uri2Ptr;
};
std::map<std::string, std::shared_ptr<DataProperty> > DataProperty::uri2Ptr;

class ObjectProperty : public Property {
public:
    // iri to Property map
    static std::map<std::string, std::shared_ptr<ObjectProperty> > uri2Ptr;
};
std::map<std::string, std::shared_ptr<ObjectProperty> > ObjectProperty::uri2Ptr;

void extractRDFS(const Object& o, RDFSEntity *rdfs) {
    rdfs->rdfname = o.iri();
    std::shared_ptr<PropertyValue> comment = o.getOptionalPropertyValue(RDFS + "#comment");
    if (comment) {
        rdfs->comment = *comment;
    }
    std::shared_ptr<PropertyValue> label = o.getOptionalPropertyValue(RDFS + "#label");
    if (label) {
        rdfs->label = *label;
    }
}

void extractProperty(const Object& o, Property *prop) {
    std::list<PropertyValue> domainList = o.getPropertyValueList(RDFS + "#domains");
    for ( const PropertyValue& value: domainList ) {
        prop->domains.push_back(value);
    }
    std::list<PropertyValue> rangeList = o.getPropertyValueList(RDFS + "#range");
    if ( rangeList.size() == 1 ) {
        prop->range = rangeList.front();
    } else if ( rangeList.size() > 1 ) {
        std::stringstream ss;
        ss << "rdfs#range has more than one item for " << o.iri();
        throw std::runtime_error(ss.str());
    }
}

void run() {
    // Gather classes
    const std::list<Object>& rdfsClasses = Object::findByType(RDFS + "#Class");
    for ( auto const& rdfsClass : rdfsClasses) {
        auto k = std::make_shared<klass>();
        extractRDFS(rdfsClass, k.get());
        klass::uri2Ptr[k->rdfname] = k;
    }

    // Gather data Properties
    const std::list<Object>& owlDataProperties = Object::findByType(OWL + "#DatatypeProperty");
    for ( auto const& owlDataProperty : owlDataProperties) {
        auto p = std::make_shared<DataProperty>();
        extractRDFS(owlDataProperty, p.get());
        DataProperty::uri2Ptr[p->rdfname] = p;
    }

    // Gather object Properties
    const std::list<Object>& owlObjectProperties = Object::findByType(OWL + "#ObjectProperty");
    for ( auto const& owlObjectProperty : owlObjectProperties) {
        auto p = std::make_shared<ObjectProperty>();
        extractRDFS(owlObjectProperty, p.get());
        ObjectProperty::uri2Ptr[p->rdfname] = p;
    }

    // Make links between properties and classes
    for ( auto const& dataPropertyMapItem : DataProperty::uri2Ptr ) {
        const DataProperty& dataProperty = *dataPropertyMapItem.second;
        for(const std::string& currentDomain : dataProperty.domains) {
            auto klassIt = klass::uri2Ptr.find(currentDomain);
            if ( klassIt != klass::uri2Ptr.end() ) {
                klassIt->second->dataProperties.push_back(dataPropertyMapItem.second);
            } else {
                std::cerr << "Property " << dataProperty.rdfname << " refers to unreachable rdfs class " << currentDomain << ", skipping" << std::endl;
            }
        }
    }
    for ( auto const& objectPropertyMapItem : ObjectProperty::uri2Ptr ) {
        const ObjectProperty& objectProperty = *objectPropertyMapItem.second;
        for(const std::string& currentDomain : objectProperty.domains) {
            auto klassIt = klass::uri2Ptr.find(currentDomain);
            if ( klassIt != klass::uri2Ptr.end() ) {
                klassIt->second->objectProperties.push_back(objectPropertyMapItem.second);
            } else {
                std::cerr << "Property " << objectProperty.rdfname << " refers to unreachable rdfs class " << currentDomain << ", skipping" << std::endl;
            }
        }
    }

    // Starting code Generation
    std::string cppNameSpace = "foaf";
    if ( ::mkdir(cppNameSpace.c_str(), S_IRWXG | S_IRWXU | S_IRWXO) != 0 ) {
        if ( errno != EEXIST ) {
            std::stringstream ss;
            ss << "Error while creating output directory: " << ::strerror(errno);
            throw std::runtime_error(ss.str());
        }
    }
    for ( auto const& klassMapItem: klass::uri2Ptr) {
        generateCode(*klassMapItem.second, cppNameSpace);
    }

    //Generate all inclusions file
    std::string fileName = cppNameSpace + "/" + cppNameSpace + ".h";
    std::ofstream ofs(fileName);
    if (!ofs.is_open()) {
        throw std::runtime_error("Unable to open " + cppNameSpace + " file");
    }

    generateCodeProptectorBegin(ofs, cppNameSpace, cppNameSpace);

    for ( auto const& klassMapItem: klass::uri2Ptr) {
        const klass& cls = *klassMapItem.second;
        ofs << "#include <" << cppNameSpace << "/" << cls.genCppName() << ".h" << ">" << std::endl;
    }
    ofs << std::endl;
    generateCodeProptectorEnd(ofs, cppNameSpace, cppNameSpace);
}

void generateCodeProptectorBegin(std::ofstream& ofs, const std::string& cppNameSpace, const std::string cppName) {
    std::string upperCppNameSpace = cppNameSpace;
    std::transform(upperCppNameSpace.begin(), upperCppNameSpace.end(), upperCppNameSpace.begin(), ::toupper);
    std::string upperClassName = cppName;
    std::transform(upperClassName.begin(), upperClassName.end(), upperClassName.begin(), ::toupper);

    std::string protector = upperCppNameSpace + "_" + upperClassName;

    ofs << "// This is auto generated code by AutoRDF, do not edit !" << std::endl;
    ofs << "#ifndef " << protector << std::endl;
    ofs << "#define " << protector << std::endl;
    ofs << std::endl;
}

void generateCodeProptectorEnd(std::ofstream& ofs, const std::string& cppNameSpace, const std::string cppName) {
    std::string upperCppNameSpace = cppNameSpace;
    std::transform(upperCppNameSpace.begin(), upperCppNameSpace.end(), upperCppNameSpace.begin(), ::toupper);
    std::string upperClassName = cppName;
    std::transform(upperClassName.begin(), upperClassName.end(), upperClassName.begin(), ::toupper);

    std::string protector = upperCppNameSpace + "_" + upperClassName;

    ofs << "#endif // " <<  protector << std::endl;
}

void generateCode(const klass& kls, const std::string& cppNameSpace) {
    std::string cppName = kls.genCppName();
    std::string fileName = cppNameSpace + "/" + cppName + ".h";
    std::ofstream ofs(fileName);
    if (!ofs.is_open()) {
        throw std::runtime_error("Unable to open " + cppNameSpace + " file");
    }

    generateCodeProptectorBegin(ofs, cppNameSpace, cppName);

    ofs << "namespace " << cppNameSpace << " {" << std::endl;
    ofs << std::endl;
    ofs << "class " << cppName << " {" << std::endl;

    ofs << "};" << std::endl;
    ofs << std::endl;
    ofs << "}" << std::endl;

    generateCodeProptectorEnd(ofs, cppNameSpace, cppName);
}

}
}


int main(int argc, char **argv) {
    autordf::Factory f;
    autordf::Object::setFactory(&f);
    f.loadFromFile(argv[1], "http://xmlns.com/foaf/0.1/Person");
    autordf::tools::run();
    return 0;
}