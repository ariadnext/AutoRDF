#include <sys/stat.h>
#include <string.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <set>
#include <map>
#include <memory>
#include <stdexcept>
#include <algorithm>

#include <autordf/Factory.h>
#include <autordf/Object.h>

namespace autordf {
namespace tools {

//FIXME: make that a parameter
std::string nameSpace = "http://xmlns.com/foaf/0.1/";

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
    std::list<PropertyValue> domainList = o.getPropertyValueList(RDFS + "#domain");
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
        extractProperty(owlDataProperty, p.get());
        DataProperty::uri2Ptr[p->rdfname] = p;
    }

    // Gather object Properties
    const std::list<Object>& owlObjectProperties = Object::findByType(OWL + "#ObjectProperty");
    for ( auto const& owlObjectProperty : owlObjectProperties) {
        auto p = std::make_shared<ObjectProperty>();
        extractRDFS(owlObjectProperty, p.get());
        extractProperty(owlObjectProperty, p.get());
        ObjectProperty::uri2Ptr[p->rdfname] = p;
    }

    // Make links between properties and classes
    std::set<std::string> wildCardProperties({OWL + "#Thing"});
    for ( auto const& dataPropertyMapItem : DataProperty::uri2Ptr ) {
        const DataProperty& dataProperty = *dataPropertyMapItem.second;
        for(const std::string& currentDomain : dataProperty.domains) {
            auto klassIt = klass::uri2Ptr.find(currentDomain);
            if ( klassIt != klass::uri2Ptr.end() ) {
                klassIt->second->dataProperties.push_back(dataPropertyMapItem.second);
            } else if ( wildCardProperties.count(currentDomain) ) {
                for ( auto& klassMapPair : klass::uri2Ptr ) {
                    klassMapPair.second->dataProperties.push_back(dataPropertyMapItem.second);
                }
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
            } else if ( wildCardProperties.count(currentDomain) ) {
                for ( auto& klassMapPair : klass::uri2Ptr ) {
                    klassMapPair.second->objectProperties.push_back(objectPropertyMapItem.second);
                }
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

    ofs << "#ifndef " << protector << std::endl;
    ofs << "#define " << protector << std::endl;
    ofs << std::endl;
    ofs << "// This is auto generated code by AutoRDF, do not edit !" << std::endl;
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

std::ostream& indent(std::ostream& os, int numIndent) {
    for (unsigned int i = 0; i < numIndent * 4; ++i) {
        os << ' ';
    }
    return os;
}

void writeRDFSEntityComment(std::ofstream& ofs, const RDFSEntity& e, unsigned int numIndent) {
    if ( !e.label.empty() || !e.comment.empty() ) {
        indent(ofs, numIndent) << "/**" << std::endl;
        if ( !e.label.empty() ) {
            indent(ofs, numIndent) << " * " << e.label << std::endl;
        }
        if ( !e.comment.empty() ) {
            indent(ofs, numIndent) << " * " << e.comment << std::endl;
        }
        indent(ofs, numIndent) << " */" << std::endl;
    }
}

void generateDataProperty(std::ofstream& ofs, const DataProperty& property) {
    ofs << std::endl;
    writeRDFSEntityComment(ofs, property, 1);
    indent(ofs, 1) << "autordf::PropertyValue " << property.genCppName() << "() const {" << std::endl;
    indent(ofs, 2) <<     "return getPropertyValue(\"" << property.rdfname << "\");" << std::endl;
    indent(ofs, 1) << "}" << std::endl;
    indent(ofs, 1) << "std::shared_ptr<autordf::PropertyValue> " << property.genCppName() << "Optional() const {" << std::endl;
    indent(ofs, 2) <<     "return getOptionalPropertyValue(\"" << property.rdfname << "\");" << std::endl;
    indent(ofs, 1) << "}" << std::endl;
    indent(ofs, 1) << "std::list<autordf::PropertyValue> " << property.genCppName() << "List() const {" << std::endl;
    indent(ofs, 2) <<     "return getPropertyValueList(\"" << property.rdfname << "\");" << std::endl;
    indent(ofs, 1) << "}" << std::endl;
}

std::string findCppClassNameForProperty(const ObjectProperty& property) {
    std::string cppClassName;
    auto kit = klass::uri2Ptr.find(property.range);
    if ( kit != klass::uri2Ptr.end() ) {
        cppClassName = kit->second->genCppName();
    }
    return cppClassName;
}

void generateObjectProperty(std::ofstream& ofs, const ObjectProperty& property) {
    // First try to test if its range refers to a known class
    std::string cppClassName = findCppClassNameForProperty(property);

    ofs << std::endl;
    writeRDFSEntityComment(ofs, property, 1);
    if ( cppClassName.empty() ) {
        indent(ofs, 1) << "std::shared_ptr<autordf::Object> " << property.genCppName() << "() const {" << std::endl;
        indent(ofs, 2) <<     "return std::make_shared<autordf::Object>(getObject(\"" << property.rdfname << "\"));" << std::endl;
        indent(ofs, 1) << "}" << std::endl;
    } else {
        indent(ofs, 1) << "std::shared_ptr<" << cppClassName << "> " << property.genCppName() << "() const {" << std::endl;
        indent(ofs, 2) <<     "return std::make_shared<" << cppClassName << ">(getObject(\"" << property.rdfname << "\"));" << std::endl;
        indent(ofs, 1) << "}" << std::endl;
    }
    if ( cppClassName.empty() ) {
        indent(ofs, 1) << "std::shared_ptr<autordf::Object> " << property.genCppName() << "Optional() const {" << std::endl;
        indent(ofs, 2) <<     "return getOptionalObject(\"" << property.rdfname << "\");" << std::endl;
        indent(ofs, 1) << "}" << std::endl;
    } else {
        indent(ofs, 1) << "std::shared_ptr<" << cppClassName << "> " << property.genCppName() << "Optional() const {" << std::endl;
        indent(ofs, 2) <<     "auto result = getOptionalObject(\"" << property.rdfname << "\");" << std::endl;
        indent(ofs, 2) <<     "return result ? std::make_shared<" << cppClassName << ">(*result) : nullptr;" << std::endl;
        indent(ofs, 1) << "}" << std::endl;
    }
    if ( cppClassName.empty() ) {
        indent(ofs, 1) << "std::list<std::shared_ptr<autordf::Object> > " << property.genCppName() << "List() const {" << std::endl;
        indent(ofs, 2) <<     "return getObjectList(\"" << property.rdfname << "\");" << std::endl;
        indent(ofs, 1) << "}" << std::endl;
    } else {
        indent(ofs, 1) << "std::list<std::shared_ptr<" << cppClassName << "> > " << property.genCppName() << "List() const {" << std::endl;
        indent(ofs, 2) <<     "return getObjectListImpl<typename std::shared_ptr<" << cppClassName << "> >(\"" << property.rdfname << "\");" << std::endl;
        indent(ofs, 1) << "}" << std::endl;
    }
}

void generateCode(const klass& kls, const std::string& cppNameSpace) {
    std::string cppName = kls.genCppName();
    std::string fileName = cppNameSpace + "/" + cppName + ".h";
    std::ofstream ofs(fileName);
    if (!ofs.is_open()) {
        throw std::runtime_error("Unable to open " + cppNameSpace + " file");
    }

    generateCodeProptectorBegin(ofs, cppNameSpace, cppName);

    ofs << "#include <autordf/Object.h>" << std::endl;
    ofs << std::endl;

    ofs << "namespace " << cppNameSpace << " {" << std::endl;
    ofs << std::endl;
    //get forward declarations includes
    std::set<std::string> cppImports;
    for ( const std::shared_ptr<ObjectProperty> p : kls.objectProperties) {
        const std::string& val = findCppClassNameForProperty(*p);
        if ( !val.empty() && (val != cppName) ) {
            cppImports.insert(val);
        }
    }
    for ( const std::string& cppClassName : cppImports ) {
        ofs << "class " << cppClassName << ";" << std::endl;
    }
    ofs << std::endl;

    writeRDFSEntityComment(ofs, kls, 0);
    ofs << "class " << cppName << ": public autordf::Object {" << std::endl;
    indent(ofs, 1) << "#define AUTORDF_RDFTYPEIRI \"" << kls.rdfname << "\"" << std::endl;
    ofs << "public:" << std::endl;
    ofs << std::endl;
    indent(ofs, 1) << "/**" << std::endl;
    indent(ofs, 1) << " * Creates new object, to given iri. If iri empty," << std::endl;
    indent(ofs, 1) << " * creates an anonymous (aka blank) object" << std::endl;
    indent(ofs, 1) << " */" << std::endl;
    indent(ofs, 1) << cppName << "(const std::string& iri = \"\") : autordf::Object(iri) {}" << std::endl;
    ofs << std::endl;
    indent(ofs, 1) << "/**" << std::endl;
    indent(ofs, 1) << " * Build us using the same underlying resource as the other object" << std::endl;
    indent(ofs, 1) << " */" << std::endl;
    indent(ofs, 1) << cppName << "(const Object& other) : autordf::Object(other) {}" << std::endl;
    ofs << std::endl;
    indent(ofs, 1) << "static std::list<" << cppName << "> find() {" << std::endl;
    indent(ofs, 2) << "return findHelper<" << cppName << ">(AUTORDF_RDFTYPEIRI);" << std::endl;
    indent(ofs, 1) << "}" << std::endl;

    for ( const std::shared_ptr<DataProperty>& prop : kls.dataProperties) {
        generateDataProperty(ofs, *prop);
    }

    for ( const std::shared_ptr<ObjectProperty>& prop : kls.objectProperties) {
        generateObjectProperty(ofs, *prop);
    }

    ofs << std::endl;
    indent(ofs, 1) << "#undef AUTORDF_RDFTYPEIRI" << std::endl;
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
    f.loadFromFile(argv[1], autordf::tools::nameSpace);
    autordf::tools::run();
    return 0;
}