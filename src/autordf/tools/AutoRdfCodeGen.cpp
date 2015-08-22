#include <sys/stat.h>
#include <string.h>
#include <getopt.h>

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

std::string outdir = ".";
bool verbose = false;
bool generateAllInOne = false;

static const std::string RDF = "http://www.w3.org/1999/02/22-rdf-syntax-ns#";
static const std::string RDFS = "http://www.w3.org/2000/01/rdf-schema#";
static const std::string OWL  = "http://www.w3.org/2002/07/owl#";

class ObjectProperty;
class DataProperty;
class klass;

std::tuple<const char *, cvt::RdfTypeEnum, const char *> rdf2CppTypeMapping[] = {
//FIXME: handle PlainLiteral, XMLLiteral, Literal, real, rational
    std::make_tuple("http://www.w3.org/2001/XMLSchema#string",             cvt::RdfTypeEnum::xsd_string,             "std::string"),
    std::make_tuple("http://www.w3.org/2001/XMLSchema#boolean",            cvt::RdfTypeEnum::xsd_boolean,            "bool"),
    std::make_tuple("http://www.w3.org/2001/XMLSchema#decimal",            cvt::RdfTypeEnum::xsd_decimal,            "double"),
    std::make_tuple("http://www.w3.org/2001/XMLSchema#float",              cvt::RdfTypeEnum::xsd_float,              "float"),
    std::make_tuple("http://www.w3.org/2001/XMLSchema#double",             cvt::RdfTypeEnum::xsd_double,             "double"),
    std::make_tuple("http://www.w3.org/2001/XMLSchema#dateTime",           cvt::RdfTypeEnum::xsd_dateTime,           "boost::posix_time::ptime"),
    std::make_tuple("http://www.w3.org/2001/XMLSchema#integer",            cvt::RdfTypeEnum::xsd_integer,            "long long int"),
    std::make_tuple("http://www.w3.org/2001/XMLSchema#dateTimeStamp",      cvt::RdfTypeEnum::xsd_dateTimeStamp,      "boost::posix_time::ptime"),
    std::make_tuple("http://www.w3.org/2001/XMLSchema#nonNegativeInteger", cvt::RdfTypeEnum::xsd_nonNegativeInteger, "long long unsigned int"),
    std::make_tuple("http://www.w3.org/2001/XMLSchema#positiveInteger",    cvt::RdfTypeEnum::xsd_positiveInteger,    "long long unsigned int"),
    std::make_tuple("http://www.w3.org/2001/XMLSchema#nonPositiveInteger", cvt::RdfTypeEnum::xsd_nonPositiveInteger, "long long int"),
    std::make_tuple("http://www.w3.org/2001/XMLSchema#negativeInteger",    cvt::RdfTypeEnum::xsd_negativeInteger,    "long long int"),
    std::make_tuple("http://www.w3.org/2001/XMLSchema#long",               cvt::RdfTypeEnum::xsd_long,               "long long int"),
    std::make_tuple("http://www.w3.org/2001/XMLSchema#unsignedLong",       cvt::RdfTypeEnum::xsd_unsignedLong,       "unsigned long long int"),
    std::make_tuple("http://www.w3.org/2001/XMLSchema#int",                cvt::RdfTypeEnum::xsd_int,                "long int"),
    std::make_tuple("http://www.w3.org/2001/XMLSchema#unsignedInt",        cvt::RdfTypeEnum::xsd_unsignedInt,        "long unsigned int"),
    std::make_tuple("http://www.w3.org/2001/XMLSchema#short",              cvt::RdfTypeEnum::xsd_short,              "short"),
    std::make_tuple("http://www.w3.org/2001/XMLSchema#unsignedShort",      cvt::RdfTypeEnum::xsd_unsignedShort,      "unsigned short"),
    std::make_tuple("http://www.w3.org/2001/XMLSchema#byte",               cvt::RdfTypeEnum::xsd_byte,               "char"),
    std::make_tuple("http://www.w3.org/2001/XMLSchema#unsignedByt",        cvt::RdfTypeEnum::xsd_unsignedByte,       "uncisnged char")
};

std::ostream& indent(std::ostream& os, int numIndent) {
    for (unsigned int i = 0; i < numIndent * 4; ++i) {
        os << ' ';
    }
    return os;
}

void addBoilerPlate(std::ofstream& ofs);
void generateCodeProptectorBegin(std::ofstream& ofs, const std::string& cppNameSpace, const std::string cppName);
void generateCodeProptectorEnd(std::ofstream& ofs, const std::string& cppNameSpace, const std::string cppName);

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

void createDirectory(const std::string& relativeDirName) {
    if ( !relativeDirName.empty() && relativeDirName != "." ) {
        if ( ::mkdir(relativeDirName.c_str(), S_IRWXG | S_IRWXU | S_IRWXO) != 0 ) {
            if ( errno != EEXIST ) {
                std::stringstream ss;
                ss << "Error while creating output directory: " << ::strerror(errno);
                throw std::runtime_error(ss.str());
            }
        }
    }
}

std::string genCppName(const std::string& iri) {
    std::string cppname = iri.substr(iri.find_last_of("/#:") + 1);
    if ( !::isalpha(cppname[0]) ) {
        cppname = "_" + cppname;
    }
    return cppname;
}

class RDFSEntity {
public:
    // Object iri
    std::string rdfname;

    // rdfs comment
    std::string comment;
    // rdfs label
    std::string label;

    std::string genCppName() const {
        return tools::genCppName(rdfname);
    }

    std::string genCppNameSpace() const {
        std::string prefix = rdfPrefix(rdfname, _m);
        if ( !prefix.empty() ) {
            return prefix;
        } else {
            throw std::runtime_error("No prefix found for " + rdfname + " RDF resource, unable to use it as C++ namespace");
        }
    }

    std::string genCppNameWithNamespace() const {
        return genCppNameSpace() + "::" + genCppName();
    }

    void generateComment(std::ofstream& ofs, unsigned int numIndent) const {
        if ( !label.empty() || !comment.empty() ) {
            indent(ofs, numIndent) << "/**" << std::endl;
            if ( !label.empty() ) {
                indent(ofs, numIndent) << " * " << label << std::endl;
            }
            if ( !comment.empty() ) {
                indent(ofs, numIndent) << " * " << comment << std::endl;
            }
            indent(ofs, numIndent) << " */" << std::endl;
        }
    }

    static void setModel(Model *m) {
        _m = m;
    }

    static const Model* model() { return _m; }

private:
    static Model *_m;
};

Model *RDFSEntity::_m = 0;

class klass : public RDFSEntity {
public:
    std::set<std::string> directAncestors;
    std::set<std::string> enumValues;

    std::set<std::shared_ptr<DataProperty> > dataProperties;
    std::set<std::shared_ptr<ObjectProperty> > objectProperties;

    // It is usual (but optional) to specify at class level the minimum and/or maximum instances for a property
    // This is done using cardiniality restrictions
    std::map<std::string, unsigned int> overridenMinCardinality;
    std::map<std::string, unsigned int> overridenMaxCardinality;
    // Qualified Cardinality restrictions also allow to specify a range for this instance properties
    std::map<std::string, std::string> overridenRange;

    std::set<std::shared_ptr<const klass> > getClassDependencies() const;

    std::set<std::shared_ptr<const klass> > getAllAncestors() const;

    void generateInterfaceDeclaration() const;

    void generateInterfaceDefinition() const;

    void generateDeclaration() const;

    // iri to klass map
    static std::map<std::string, std::shared_ptr<klass> > uri2Ptr;
};
std::map<std::string, std::shared_ptr<klass> > klass::uri2Ptr;

class Property : public RDFSEntity {
public:
    std::list<std::string> domains;
    std::string range;
    unsigned int minCardinality;
    unsigned int maxCardinality;

    //FIXME: this behaviour should be checked against the standard
    unsigned int getEffectiveMinCardinality(const klass& kls) const {
        auto it = kls.overridenMinCardinality.find(rdfname);
        if ( it != kls.overridenMinCardinality.end() ) {
            return  it->second;
        }
        return minCardinality;
    }

    //FIXME: this behaviour should be checked against the standard
    unsigned int getEffectiveMaxCardinality(const klass& kls) const {
        auto it = kls.overridenMaxCardinality.find(rdfname);
        if ( it != kls.overridenMaxCardinality.end() ) {
            return  it->second;
        }
        return maxCardinality;
    }
};

class DataProperty : public Property {
public:
    // iri to Property map
    static std::map<std::string, std::shared_ptr<DataProperty> > uri2Ptr;

    void generateDeclaration(std::ofstream& ofs, const klass& onClass) const {
        ofs << std::endl;
        generateComment(ofs, 1);
        if ( getEffectiveMaxCardinality(onClass) <= 1 ) {
            if ( getEffectiveMinCardinality(onClass) > 0 ) {
                generateForOneMandatory(ofs, onClass);
            } else {
                generateForOneOptional(ofs, onClass);
            }
        }
        if ( getEffectiveMaxCardinality(onClass) > 1 ) {
            generateForMany(ofs, onClass);
        }
    }

private:
    std::string getEffectiveRange(const klass& kls) const {
        auto it = kls.overridenRange.find(rdfname);
        if ( it != kls.overridenRange.end() ) {
            return  it->second;
        }
        return range;
    }

    int range2CvtArrayIndex(const klass& onClass) const {
        std::string effectiveRange = getEffectiveRange(onClass);
        if ( !effectiveRange.empty() ) {
            for ( unsigned int i = 0; i < (sizeof(rdf2CppTypeMapping) / sizeof(rdf2CppTypeMapping[0])); ++i ) {
                if ( std::get<0>(rdf2CppTypeMapping[i]) == effectiveRange ) {
                    return i;
                }
            }
            std::cerr << effectiveRange << " type not supported, we will default to raw value for property " << rdfname << std::endl;
        }
        return -1;
    }


    void generateForOneMandatory(std::ofstream& ofs, const klass& onClass) const {
        int index = range2CvtArrayIndex(onClass);
        if ( index >= 0 ) {
            const char *cppType = std::get<2>(rdf2CppTypeMapping[index]);
            cvt::RdfTypeEnum rdfType = std::get<1>(rdf2CppTypeMapping[index]);
            indent(ofs, 1) << cppType << " " << genCppName() << "() const {" << std::endl;
            indent(ofs, 2) << "return object().getPropertyValue(\"" << rdfname << "\").get<autordf::cvt::RdfTypeEnum::" << cvt::rdfTypeEnumString(rdfType) << ", " << cppType<< ">();" << std::endl;
            indent(ofs, 1) << "}" << std::endl;
        } else {
            indent(ofs, 1) << "autordf::PropertyValue " << genCppName() << "() const {" << std::endl;
            indent(ofs, 2) << "return object().getPropertyValue(\"" << rdfname << "\");" << std::endl;
            indent(ofs, 1) << "}" << std::endl;
        }
    }

    void generateForOneOptional(std::ofstream& ofs, const klass& onClass) const {
        int index = range2CvtArrayIndex(onClass);
        if ( index >= 0 ) {
            const char *cppType = std::get<2>(rdf2CppTypeMapping[index]);
            cvt::RdfTypeEnum rdfType = std::get<1>(rdf2CppTypeMapping[index]);
            indent(ofs, 1) << "std::shared_ptr<" << cppType << "> " << genCppName() << "Optional() const {" << std::endl;
            indent(ofs, 2) << "auto ptrval = object().getOptionalPropertyValue(\"" << rdfname << "\");" << std::endl;
            indent(ofs, 2) << "return (ptrval ? std::shared_ptr<" << cppType << ">(new " << cppType << "(ptrval->get<autordf::cvt::RdfTypeEnum::" << cvt::rdfTypeEnumString(rdfType) << ", " << cppType << ">())) : nullptr);"<< std::endl;
            indent(ofs, 1) << "}" << std::endl;
        } else {
            indent(ofs, 1) << "std::shared_ptr<autordf::PropertyValue> " << genCppName() << "Optional() const {" << std::endl;
            indent(ofs, 2) << "return object().getOptionalPropertyValue(\"" << rdfname << "\");" << std::endl;
            indent(ofs, 1) << "}" << std::endl;
        }
    }

    void generateForMany(std::ofstream& ofs, const klass& onClass) const {
        int index = range2CvtArrayIndex(onClass);
        if ( index >= 0 ) {
            const char *cppType = std::get<2>(rdf2CppTypeMapping[index]);
            cvt::RdfTypeEnum rdfType = std::get<1>(rdf2CppTypeMapping[index]);
            indent(ofs, 1) << "std::list<" << cppType << "> " << genCppName() << "List() const {" << std::endl;
            indent(ofs, 2) <<     "auto pValuelist = object().getPropertyValueList(\"" << rdfname << "\");" << std::endl;
            indent(ofs, 2) <<     "std::list<" << cppType << "> list;"<<  std::endl;
            indent(ofs, 2) <<     "for ( const autordf::PropertyValue& pv : pValuelist ) {"<<  std::endl;
            indent(ofs, 3) <<         "list.push_back(pv.get<autordf::cvt::RdfTypeEnum::" << cvt::rdfTypeEnumString(rdfType) << ", " << cppType<< ">());" <<  std::endl;
            indent(ofs, 2) <<     "}"<<  std::endl;
            indent(ofs, 2) <<     "return list;"<<  std::endl;
            indent(ofs, 1) << "}" << std::endl;
        } else {
            indent(ofs, 1) << "std::list<autordf::PropertyValue> " << genCppName() << "List() const {" << std::endl;
            indent(ofs, 2) <<     "return object().getPropertyValueList(\"" << rdfname << "\");" << std::endl;
            indent(ofs, 1) << "}" << std::endl;
        }
    }
};
std::map<std::string, std::shared_ptr<DataProperty> > DataProperty::uri2Ptr;

class ObjectProperty : public Property {
public:

    // Returns class for this property, or nullptr if no class is registered
    std::shared_ptr<klass> findClass() const {
        auto kit = klass::uri2Ptr.find(range);
        if ( kit != klass::uri2Ptr.end() ) {
            return kit->second;
        }
        return nullptr;
    }

    void generateDeclaration(std::ofstream& ofs, const klass& onClass) const {
        auto propertyClass = findClass();

        ofs << std::endl;
        generateComment(ofs, 1);

        if ( getEffectiveMaxCardinality(onClass) <= 1 ) {
            if ( getEffectiveMinCardinality(onClass) > 0 ) {
                indent(ofs, 1) << propertyClass->genCppNameWithNamespace() << " " << genCppName() << "() const;" << std::endl;
            } else {
                indent(ofs, 1) << "std::shared_ptr<" << propertyClass->genCppNameWithNamespace()  << "> " << genCppName() << "Optional() const;" << std::endl;
            }
        }
        if ( getEffectiveMaxCardinality(onClass) > 1 ) {
            indent(ofs, 1) << "std::list<" << propertyClass->genCppNameWithNamespace()  << "> " << genCppName() << "List() const;" << std::endl;
        }
    }

    void generateDefinition(std::ofstream& ofs, const klass& onClass) {
        auto propertyClass = findClass();
        std::string currentClassName = "I" + onClass.genCppName();

        if ( getEffectiveMaxCardinality(onClass) <= 1 ) {
            if ( getEffectiveMinCardinality(onClass) > 0 ) {
                ofs << propertyClass->genCppNameWithNamespace() << " " << currentClassName << "::" << genCppName() << "() const {" << std::endl;
                indent(ofs, 1) << "return object().getObject(\"" << rdfname << "\").as<" << propertyClass->genCppNameWithNamespace() << ">();" << std::endl;
                ofs << "}" << std::endl;
                ofs << std::endl;
            } else {
                ofs << "std::shared_ptr<" << propertyClass->genCppNameWithNamespace() << "> " << currentClassName << "::" << genCppName() << "Optional() const {" << std::endl;
                indent(ofs, 1) << "auto result = object().getOptionalObject(\"" << rdfname << "\");" << std::endl;
                indent(ofs, 1) << "return result ? std::make_shared<" << propertyClass->genCppNameWithNamespace() << ">(*result) : nullptr;" << std::endl;
                ofs << "}" << std::endl;
                ofs << std::endl;
            }
        }
        if ( getEffectiveMaxCardinality(onClass) > 1 ) {
            ofs << "std::list<" << propertyClass->genCppNameWithNamespace() << "> " << currentClassName << "::" << genCppName() << "List() const {" << std::endl;
            indent(ofs, 1) << "return object().getObjectListImpl<" << propertyClass->genCppNameWithNamespace() << ">(\"" <<  rdfname << "\");" << std::endl;
            ofs << "}" << std::endl;
            ofs << std::endl;
        }
    }

    // iri to Property map
    static std::map<std::string, std::shared_ptr<ObjectProperty> > uri2Ptr;
};
std::map<std::string, std::shared_ptr<ObjectProperty> > ObjectProperty::uri2Ptr;

std::set<std::shared_ptr<const klass> > klass::getClassDependencies() const {
    std::set<std::shared_ptr<const klass> > deps;
    for ( const std::shared_ptr<ObjectProperty> p : objectProperties) {
        auto val = p->findClass();
        if ( val && (val->genCppName() != genCppName()) ) {
            deps.insert(val);
        }
    }
    return deps;
}

std::set<std::shared_ptr<const klass> > klass::getAllAncestors() const {
    std::set<std::shared_ptr<const klass> > all;
    for ( auto ancestor = directAncestors.begin(); ancestor != directAncestors.end(); ++ancestor ) {
        all.insert(uri2Ptr[*ancestor]);
        for ( std::shared_ptr<const klass> more : uri2Ptr[*ancestor]->getAllAncestors() ) {
            all.insert(more);
        }
    }
    return all;
}

void klass::generateDeclaration() const {
    std::string cppName = genCppName();
    std::string fileName = outdir + "/" + genCppNameSpace() + "/" + cppName + ".h";
    std::ofstream ofs(fileName);
    if (!ofs.is_open()) {
        throw std::runtime_error("Unable to open " + fileName + " file");
    }

    generateCodeProptectorBegin(ofs, genCppNameSpace(), cppName);

    ofs << "#include <autordf/Object.h>" << std::endl;
    ofs << "#include <" << genCppNameSpace() << "/I" << cppName << ".h>" << std::endl;
    for ( const std::string& ancestor: directAncestors ) {
        ofs << "#include <" << uri2Ptr[ancestor]->genCppNameSpace() << "/I" << uri2Ptr[ancestor]->genCppName() << ".h>" << std::endl;
    }
    ofs << std::endl;

    ofs << "namespace " << genCppNameSpace() << " {" << std::endl;
    ofs << std::endl;

    generateComment(ofs, 0);
    ofs << "class " << cppName << ": public autordf::Object";
    for ( auto ancestor = directAncestors.begin(); ancestor != directAncestors.end(); ++ancestor ) {
        ofs << ", public " << uri2Ptr[*ancestor]->genCppNameSpace() << "::I" << uri2Ptr[*ancestor]->genCppName();
    }
    ofs << ", public I" << cppName << " {" << std::endl;
    ofs << "public:" << std::endl;
    ofs << std::endl;
    if ( !enumValues.size() ) {
        indent(ofs, 1) << "/**" << std::endl;
        indent(ofs, 1) << " * Creates new object, to given iri. If iri empty," << std::endl;
        indent(ofs, 1) << " * creates an anonymous (aka blank) object" << std::endl;
        indent(ofs, 1) << " */" << std::endl;
        indent(ofs, 1) << cppName << "(const std::string& iri = \"\") : autordf::Object(iri) {}" << std::endl;
    } else {
        indent(ofs, 1) << "/**" << std::endl;
        indent(ofs, 1) << " * Load enum from RDF model, from given C++ Type enum." << std::endl;
        indent(ofs, 1) << " * This applies only to classes defines using the owl:oneOf paradigm" << std::endl;
        indent(ofs, 1) << " */" << std::endl;
        indent(ofs, 1) << cppName << "(I" << cppName << "::Enum enumVal) : autordf::Object(enumIri(enumVal)) {}" << std::endl;
    }
    ofs << std::endl;
    indent(ofs, 1) << "/**" << std::endl;
    indent(ofs, 1) << " * Build us using the same underlying resource as the other object" << std::endl;
    indent(ofs, 1) << " */" << std::endl;
    indent(ofs, 1) << cppName << "(const Object& other) : autordf::Object(other) {}" << std::endl;
    ofs << std::endl;
    indent(ofs, 1) << "static std::list<" << cppName << "> find() {" << std::endl;
    indent(ofs, 2) << "return findHelper<" << cppName << ">(I" << cppName << "::TYPEIRI);" << std::endl;
    indent(ofs, 1) << "}" << std::endl;
    ofs << std::endl;

    ofs << "private:" << std::endl;

    indent(ofs, 1) << "Object& object() { return *this; }" << std::endl;
    indent(ofs, 1) << "const Object& object() const { return *this; }" << std::endl;
    ofs << "};" << std::endl;
    ofs << std::endl;
    ofs << "}" << std::endl;

    generateCodeProptectorEnd(ofs, genCppNameSpace(), cppName);
}

void klass::generateInterfaceDeclaration() const {
    std::string cppName = "I" + genCppName();
    std::string fileName = outdir + "/" + genCppNameSpace() + "/" + cppName + ".h";
    std::ofstream ofs(fileName);
    if (!ofs.is_open()) {
        throw std::runtime_error("Unable to open " + fileName + " file");
    }

    generateCodeProptectorBegin(ofs, genCppNameSpace(), cppName);

    if ( enumValues.size() ) {
        ofs << "#include <array>" << std::endl;
        ofs << "#include <tuple>" << std::endl;
        ofs << std::endl;
    }
    ofs << "#include <autordf/Object.h>" << std::endl;
    ofs << std::endl;

    //get forward declarations
    std::set<std::shared_ptr<const klass> > cppClassDeps = getClassDependencies();
    for ( const std::shared_ptr<const klass>& cppClassDep : cppClassDeps ) {
        ofs << "namespace " << cppClassDep->genCppNameSpace() << " { class " << cppClassDep->genCppName() << "; }" << std::endl;
    }
    // Add forward declaration for our own class
    ofs << "namespace " << genCppNameSpace() << " { class " << genCppName() << "; }" << std::endl;
    ofs << std::endl;

    ofs << "namespace " << genCppNameSpace() << " {" << std::endl;
    ofs << std::endl;

    generateComment(ofs, 0);
    ofs << "class " << cppName << " ";
    ofs << " {" << std::endl;
    ofs << "public:" << std::endl;
    indent(ofs, 1) << "// IRI for rfds type name" << std::endl;
    indent(ofs, 1) << "static const std::string& TYPEIRI;" << std::endl;

    if ( enumValues.size() ) {
        indent(ofs, 1) << "enum Enum {" << std::endl;
        for ( std::string en : enumValues ) {
            indent(ofs, 2) << tools::genCppName(en) << "," << std::endl;
        }
        indent(ofs, 1) << "};" << std::endl;
        ofs << std::endl;
        indent(ofs, 1) << "Enum asEnum() const;" << std::endl;
        ofs << std::endl;
        indent(ofs, 1) << "static std::string enumString(Enum en);" << std::endl;
    }
    ofs << std::endl;
    for ( const std::shared_ptr<DataProperty>& prop : dataProperties) {
        prop->generateDeclaration(ofs, *this);
    }

    for ( const std::shared_ptr<ObjectProperty>& prop : objectProperties) {
        prop->generateDeclaration(ofs, *this);
    }
    ofs << std::endl;

    ofs << "private:" << std::endl;
    indent(ofs, 1) << "virtual autordf::Object& object() = 0;" << std::endl;
    indent(ofs, 1) << "virtual const autordf::Object& object() const = 0;" << std::endl;
    ofs << std::endl;
    if ( enumValues.size() ) {
        indent(ofs, 1) << "typedef std::array<std::tuple<Enum, const char *, const char *>, " << enumValues.size() << "> EnumArrayType;" << std::endl;
        indent(ofs, 1) << "static const EnumArrayType ENUMARRAY;" << std::endl;
        ofs << std::endl;
        ofs << "protected:" << std::endl;
        indent(ofs, 1) << "static std::string enumIri(Enum en);" << std::endl;
    }

    ofs << "};" << std::endl;
    ofs << std::endl;
    ofs << "}" << std::endl;

    generateCodeProptectorEnd(ofs, genCppNameSpace(), cppName);
}

void klass::generateInterfaceDefinition() const {
    std::string cppName = "I" + genCppName();
    std::string cppNameSpace = genCppNameSpace();
    std::string fileName = outdir + "/" + cppNameSpace + "/" + cppName + ".cpp";
    std::ofstream ofs(fileName);
    if (!ofs.is_open()) {
        throw std::runtime_error("Unable to open " + fileName + " file");
    }

    ofs << "#include <" << cppNameSpace << "/" << cppName << ".h>" << std::endl;
    ofs << std::endl;
    addBoilerPlate(ofs);
    ofs << std::endl;
    ofs << "#include <sstream>" << std::endl;
    ofs << std::endl;

    // Generate class imports
    std::set<std::shared_ptr<const klass> > cppDeps = getClassDependencies();
    for ( const std::shared_ptr<const klass>& cppDep : cppDeps ) {
        ofs << "#include <" << cppDep->genCppNameSpace() << "/" << cppDep->genCppName() << ".h>" << std::endl;
    }
    // Include our own class
    ofs << "#include <" << cppNameSpace << "/" << genCppName() << ".h>" << std::endl;
    ofs << std::endl;

    ofs << "namespace " << cppNameSpace << " {" << std::endl;
    ofs << std::endl;

    if ( enumValues.size() ) {
        ofs << "const " << genCppName() << "::EnumArrayType I" << genCppName() << "::ENUMARRAY = {" << std::endl;
        for ( std::string en : enumValues) {
            indent(ofs, 1) << "std::make_tuple(I" <<  genCppName() << "::" << tools::genCppName(en) << ", \"" << en << "\", \"" << tools::genCppName(en) << "\")," << std::endl;
        }
        ofs << "};" << std::endl;
        ofs << std::endl;
        ofs << "I" << genCppName() << "::Enum I" << genCppName() << "::asEnum() const {" << std::endl;
        indent(ofs, 1) << "for ( auto const& enumItem: ENUMARRAY) {" << std::endl;
        indent(ofs, 2) << "if ( object().iri() == std::get<1>(enumItem) ) return std::get<0>(enumItem);" << std::endl;
        indent(ofs, 1) << "}" << std::endl;
        indent(ofs, 1) << "throw std::runtime_error(object().iri() + \"does not point to a valid individual for C++ enum " << genCppName() << "\");" << std::endl;
        ofs << "}" << std::endl;
        ofs << std::endl;
        ofs << "std::string I" << genCppName() << "::enumIri(Enum enumVal) {" << std::endl;
        indent(ofs, 1) << "for ( auto const& enumItem: ENUMARRAY) {" << std::endl;
        indent(ofs, 2) << "if ( enumVal == std::get<0>(enumItem) ) return std::get<1>(enumItem);" << std::endl;
        indent(ofs, 1) << "}" << std::endl;
        indent(ofs, 1) << "std::stringstream ss;" << std::endl;
        indent(ofs, 1) << "ss << \"Enum value \" << enumVal << \" is not valid for for C++ enum " << genCppName() << "\";" << std::endl;
        indent(ofs, 1) << "throw std::runtime_error(ss.str());" << std::endl;
        ofs << "}" << std::endl;
        ofs << std::endl;
        ofs << "std::string I" << genCppName() << "::enumString(Enum enumVal) {" << std::endl;
        indent(ofs, 1) << "for ( auto const& enumItem: ENUMARRAY) {" << std::endl;
        indent(ofs, 2) << "if ( enumVal == std::get<0>(enumItem) ) return std::get<2>(enumItem);" << std::endl;
        indent(ofs, 1) << "}" << std::endl;
        indent(ofs, 1) << "std::stringstream ss;" << std::endl;
        indent(ofs, 1) << "ss << \"Enum value \" << enumVal << \" is not valid for for C++ enum " << genCppName() << "\";" << std::endl;
        indent(ofs, 1) << "throw std::runtime_error(ss.str());" << std::endl;
        ofs << "}" << std::endl;
        ofs << std::endl;
    }

    ofs << "const std::string& " << cppName << "::TYPEIRI = \"" << rdfname << "\";" << std::endl;
    ofs << std::endl;

    for ( const std::shared_ptr<ObjectProperty>& prop : objectProperties) {
        prop->generateDefinition(ofs, *this);
    }
    ofs << "}" << std::endl;
}


void addBoilerPlate(std::ofstream& ofs) {
    ofs << "// This is auto generated code by AutoRDF, do not edit !" << std::endl;
}

void extractRDFS(const Object& o, RDFSEntity *rdfs) {
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

void extractClassCardinality(const Object& o, klass *kls, const char * card, const char * minCard, const char * maxCard) {
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

void extractClass(const Object& o, klass *kls) {
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
    auto k = std::make_shared<klass>();
    extractRDFS(rdfsClass, k.get());
    extractClass(rdfsClass, k.get());
    klass::uri2Ptr[k->rdfname] = k;
}

void extractClasses(const std::string& classTypeIRI) {
    const std::list<Object>& classes = Object::findByType(classTypeIRI);
    for ( auto const& rdfsclass : classes) {
        if ( rdfsclass.iri().length() ) {
            if ( klass::uri2Ptr.find(rdfsclass.iri()) == klass::uri2Ptr.end() ) {
                if ( !rdfPrefix(rdfsclass.iri(), klass::model()).empty() ) {
                    if ( verbose ) {
                        std::cout << "Found class " << rdfsclass.iri() << std::endl;
                    }
                    auto k = std::make_shared<klass>();
                    extractRDFS(rdfsclass, k.get());
                    extractClass(rdfsclass, k.get());
                    klass::uri2Ptr[k->rdfname] = k;
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

void run() {
    // A well known classes:
    // FIXME add coments
    auto owlThing = std::make_shared<klass>();
    owlThing->rdfname = OWL + "Thing";
    klass::uri2Ptr[owlThing->rdfname] = owlThing;

    auto rdfsResource = std::make_shared<klass>();
    rdfsResource->rdfname = RDFS + "Resource";
    rdfsResource->directAncestors.insert(owlThing->rdfname);
    klass::uri2Ptr[rdfsResource->rdfname] = rdfsResource;

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
    for ( auto const& klasses : klass::uri2Ptr) {
        std::set<std::string>& directAncestors = klasses.second->directAncestors;
        for ( auto ancestor = directAncestors.begin(); ancestor != directAncestors.end(); ) {
            if ( klass::uri2Ptr.find(*ancestor) == klass::uri2Ptr.end() ) {
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
            auto klassIt = klass::uri2Ptr.find(currentDomain);
            if ( klassIt != klass::uri2Ptr.end() ) {
                klassIt->second->dataProperties.insert(dataPropertyMapItem.second);
            } else  {
                std::cerr << "Property " << dataProperty.rdfname << " refers to unreachable rdfs class " << currentDomain << ", skipping" << std::endl;
            }
        }
    }
    for ( auto const& objectPropertyMapItem : ObjectProperty::uri2Ptr ) {
        const ObjectProperty& objectProperty = *objectPropertyMapItem.second;
        for(const std::string& currentDomain : objectProperty.domains) {
            auto klassIt = klass::uri2Ptr.find(currentDomain);
            if ( klassIt != klass::uri2Ptr.end() ) {
                klassIt->second->objectProperties.insert(objectPropertyMapItem.second);
            } else  {
                std::cerr << "Property " << objectProperty.rdfname << " refers to unreachable rdfs class " << currentDomain << ", skipping" << std::endl;
            }
        }
    }

    // Starting code Generation
    std::set<std::string> cppNameSpaces;
    for ( auto const& klassMapItem: klass::uri2Ptr) {
        // created directory if needed
        std::string cppNameSpace = klassMapItem.second->genCppNameSpace();
        cppNameSpaces.insert(cppNameSpace);
        createDirectory( outdir + "/" + cppNameSpace);

        klassMapItem.second->generateInterfaceDeclaration();
        klassMapItem.second->generateInterfaceDefinition();
        klassMapItem.second->generateDeclaration();
    }

    //Generate all inclusions file
    for ( const std::string& cppNameSpace : cppNameSpaces ) {
        std::string fileName = outdir + "/" + cppNameSpace + "/" + cppNameSpace + ".h";
        std::ofstream ofs(fileName);
        if (!ofs.is_open()) {
            throw std::runtime_error("Unable to open " + fileName + " file");
        }

        generateCodeProptectorBegin(ofs, cppNameSpace, cppNameSpace);
        for ( auto const& klassMapItem: klass::uri2Ptr) {
            if ( klassMapItem.second->genCppNameSpace() == cppNameSpace ) {
                const klass& cls = *klassMapItem.second;
                ofs << "#include <" << cppNameSpace << "/" << cls.genCppName() << ".h" << ">" << std::endl;
            }
        }
        ofs << std::endl;
        generateCodeProptectorEnd(ofs, cppNameSpace, cppNameSpace);
    }

    //Generate all in one cpp file
    if ( generateAllInOne ) {
        std::string fileName = outdir + "/AllInOne.cpp";
        std::ofstream ofs(fileName);
        if (!ofs.is_open()) {
            throw std::runtime_error("Unable to open " + fileName + " file");
        }

        for ( auto const& klassMapItem: klass::uri2Ptr) {
            const klass& cls = *klassMapItem.second;
            ofs << "#include \"" << cls.genCppNameSpace() << "/I" << cls.genCppName() << ".cpp" << "\"" << std::endl;
        }
        ofs << std::endl;
    }
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
    addBoilerPlate(ofs);
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
                    if ( autordf::tools::verbose ) {
                        std::cout << "Adding  " << prefix << "-->" << ns << " map." << std::endl;
                    }
                    f.addNamespacePrefix(prefix, ns);
                }
                break;
            }
            case 'o':
                autordf::tools::outdir = optarg;
                break;
            case 'v':
                autordf::tools::verbose = true;
                break;
            case 'a':
                autordf::tools::generateAllInOne = true;
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

    autordf::tools::createDirectory(autordf::tools::outdir);

    // Hardcode some prefixes
    f.addNamespacePrefix("owl", autordf::tools::OWL);
    f.addNamespacePrefix("rdfs", autordf::tools::RDFS);
    //FIXME: Read that from command line
    std::string baseURI = "http://";

    autordf::tools::RDFSEntity::setModel(&f);
    autordf::Object::setFactory(&f);
    while ( optind < argc ) {
        if ( autordf::tools::verbose ) {
            std::cout << "Loading " << argv[optind] << " into model." << std::endl;
        }
        f.loadFromFile(argv[optind], baseURI);
        optind++;
    }
    autordf::tools::run();
    return 0;
}

//FIXME: Add inverse functional  / functional to model
//    Handle functional