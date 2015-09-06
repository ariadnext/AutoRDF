#include "Klass.h"

#include <fstream>

#include "Utils.h"

namespace autordf {
namespace codegen {

std::map <std::string, std::shared_ptr<Klass>> Klass::uri2Ptr;

void Klass::generateDeclaration() const {
    std::string cppName = genCppName();
    std::ofstream ofs;
    createFile(genCppNameSpaceInclusionPath() + "/" + cppName + ".h", &ofs);

    generateCodeProtectorBegin(ofs, genCppNameSpaceForGuard(), cppName);

    ofs << "#include <set>" << std::endl;
    ofs << "#include <autordf/Object.h>" << std::endl;
    ofs << "#include <" << genCppNameSpaceInclusionPath() << "/I" << cppName << ".h>" << std::endl;
    for ( const std::string& ancestor: directAncestors ) {
        ofs << "#include <" << uri2Ptr[ancestor]->genCppNameSpaceInclusionPath() << "/I" << uri2Ptr[ancestor]->genCppName() << ".h>" << std::endl;
    }
    ofs << std::endl;

    enterNameSpace(ofs);
    ofs << std::endl;

    generateComment(ofs, 0);
    ofs << "class " << cppName << ": public autordf::Object";
    for ( auto ancestor = directAncestors.begin(); ancestor != directAncestors.end(); ++ancestor ) {
        ofs << ", public " << uri2Ptr[*ancestor]->genCppNameSpaceFullyQualified() << "::I" << uri2Ptr[*ancestor]->genCppName();
    }
    ofs << ", public I" << cppName << " {" << std::endl;
    ofs << "public:" << std::endl;
    ofs << std::endl;
    if ( !enumValues.size() ) {
        indent(ofs, 1) << "/**" << std::endl;
        indent(ofs, 1) << " * Creates new object, to given iri. If iri empty," << std::endl;
        indent(ofs, 1) << " * creates an anonymous (aka blank) object" << std::endl;
        indent(ofs, 1) << " */" << std::endl;
        indent(ofs, 1) << cppName << "(const std::string& iri = \"\");" << std::endl;
    } else {
        indent(ofs, 1) << "/**" << std::endl;
        indent(ofs, 1) << " * Load enum from RDF model, from given C++ Type enum." << std::endl;
        indent(ofs, 1) << " * This applies only to classes defines using the owl:oneOf paradigm" << std::endl;
        indent(ofs, 1) << " */" << std::endl;
        indent(ofs, 1) << cppName << "(I" << cppName << "::Enum enumVal);" << std::endl;
    }
    ofs << std::endl;
    indent(ofs, 1) << "/**" << std::endl;
    indent(ofs, 1) << " * Build us using the same underlying resource as the other object" << std::endl;
    indent(ofs, 1) << " */" << std::endl;
    indent(ofs, 1) << cppName << "(const Object& other);" << std::endl;
    ofs << std::endl;
    indent(ofs, 1) << "/**" << std::endl;
    indent(ofs, 1) << " * Returns the list of all objects of this kind in the store" << std::endl;
    indent(ofs, 1) << " */" << std::endl;
    indent(ofs, 1) << "static std::list<" << cppName << "> find();" << std::endl;
    ofs << std::endl;
    indent(ofs, 1) << "/**" << std::endl;
    indent(ofs, 1) << " * Internal: returns full list of ancestors we have" << std::endl;
    indent(ofs, 1) << " **/" << std::endl;
    indent(ofs, 1) << "static std::set<std::string> ancestorsRdfTypeIRI();" << std::endl;
    ofs << std::endl;

    ofs << "private:" << std::endl;

    indent(ofs, 1) << "Object& object() override { return *this; }" << std::endl;
    indent(ofs, 1) << "const Object& object() const override { return *this; }" << std::endl;
    ofs << "};" << std::endl;
    ofs << std::endl;
    leaveNameSpace(ofs);

    generateCodeProtectorEnd(ofs, genCppNameSpaceFullyQualified(), cppName);
}

void Klass::generateDefinition() const {
    std::string cppName = genCppName();
    std::string cppNameSpace = genCppNameSpaceFullyQualified();
    std::ofstream ofs;
    createFile(genCppNameSpaceInclusionPath() + "/" + cppName + ".cpp", &ofs);

    ofs << "#include <" << genCppNameSpaceInclusionPath() << "/" << cppName << ".h>" << std::endl;
    ofs << std::endl;
    addBoilerPlate(ofs);
    ofs << std::endl;

    if ( outdir == "." ) {
        ofs << "#include \"RdfTypeInfo.h\"" << std::endl;
    } else {
        ofs << "#include \"" << outdir << "/RdfTypeInfo.h\"" << std::endl;
    }
    ofs << std::endl;

    enterNameSpace(ofs);
    ofs << std::endl;
    if ( !enumValues.size() ) {
        ofs << cppName << "::" << cppName << "(const std::string& iri) : autordf::Object(iri, I" << cppName << "::TYPEIRI, &RdfTypeInfo::data()) {" << std::endl;
        ofs << "}" << std::endl;
    } else {
        ofs << cppName << "::"<< cppName << "(I" << cppName << "::Enum enumVal) : autordf::Object(enumIri(enumVal)) {}" << std::endl;
    }
    ofs << std::endl;
    ofs << cppName << "::" << cppName << "(const Object& other) : autordf::Object(other, I" << cppName << "::TYPEIRI, &RdfTypeInfo::data()) {" << std::endl;
    ofs << "}" << std::endl;
    ofs << std::endl;
    ofs << "std::list<" << cppName << "> " << cppName << "::find() {" << std::endl;
    indent(ofs, 1) << "return findHelper<" << cppName << ">(I" << cppName << "::TYPEIRI);" << std::endl;
    ofs << "}" << std::endl;
    ofs << std::endl;
    ofs << "std::set<std::string> " << cppName << "::ancestorsRdfTypeIRI() {" << std::endl;
    indent(ofs, 1) <<     "return std::set<std::string>({" << std::endl;
    for ( const std::shared_ptr<const Klass>& ancestor : getAllAncestors() ) {
        indent(ofs, 3) << ancestor->genCppNameSpaceFullyQualified() << "::I" << ancestor->genCppName() << "::TYPEIRI," << std::endl;
    }
    indent(ofs, 2) <<         "});" << std::endl;
    ofs << "}" << std::endl;
    ofs << std::endl;
    leaveNameSpace(ofs);
    ofs << std::endl;
}

void Klass::generateInterfaceDeclaration() const {
    std::string cppName = "I" + genCppName();
    std::ofstream ofs;
    createFile(genCppNameSpaceInclusionPath() + "/" + cppName + ".h", &ofs);

    generateCodeProtectorBegin(ofs, genCppNameSpaceForGuard(), cppName);

    if ( enumValues.size() ) {
        ofs << "#include <array>" << std::endl;
        ofs << "#include <tuple>" << std::endl;
        ofs << std::endl;
    }
    ofs << "#include <autordf/Object.h>" << std::endl;
    ofs << std::endl;

    //get forward declarations
    std::set<std::shared_ptr<const Klass> > cppClassDeps = getClassDependencies();
    for ( const std::shared_ptr<const Klass>& cppClassDep : cppClassDeps ) {
        cppClassDep->enterNameSpace(ofs);
        ofs << "class " << cppClassDep->genCppName() << ";" << std::endl;
        cppClassDep->leaveNameSpace(ofs);
    }
    // Add forward declaration for our own class
    enterNameSpace(ofs);
    ofs << "class " << genCppName() << ";" << std::endl;
    leaveNameSpace(ofs);
    ofs << std::endl;

    enterNameSpace(ofs);
    ofs << std::endl;

    generateComment(ofs, 0);
    ofs << "class " << cppName << " ";
    ofs << " {" << std::endl;
    ofs << "public:" << std::endl;
    indent(ofs, 1) << "/**" << std::endl;
    indent(ofs, 1) << " * IRI for rdfs type associated with this C++ Interface class" << std::endl;
    indent(ofs, 1) << " */ " << std::endl;
    indent(ofs, 1) << "static const char* TYPEIRI;" << std::endl;
    ofs << std::endl;

    if ( enumValues.size() ) {
        indent(ofs, 1) << "/**" << std::endl;
        indent(ofs, 1) << " * C++ Enum values mapping the owl instances restrictions for this class " << std::endl;
        indent(ofs, 1) << " */ " << std::endl;
        indent(ofs, 1) << "enum Enum {" << std::endl;
        for ( const RdfsEntity& enumVal : enumValues ) {
            enumVal.generateComment(ofs, 2);
            indent(ofs, 2) << enumVal.genCppName() << "," << std::endl;
        }
        indent(ofs, 1) << "};" << std::endl;
        ofs << std::endl;

        indent(ofs, 1) << "/** " << std::endl;
        indent(ofs, 1) << " * Returns the current object as an Enum " << std::endl;
        indent(ofs, 1) << " * This object is expected to be one the instance allowed by the owl:oneOf definition." << std::endl;
        indent(ofs, 1) << " * @return enum value" << std::endl;
        indent(ofs, 1) << " * @throw InvalidEnum if the object we try to convert is not one of the instances defined by owl:oneOf" << std::endl;
        indent(ofs, 1) << " */ " << std::endl;
        indent(ofs, 1) << "Enum asEnum() const;" << std::endl;
        ofs << std::endl;
        indent(ofs, 1) << "/** " << std::endl;
        indent(ofs, 1) << " * Converts an enum value to a pretty string " << std::endl;
        indent(ofs, 1) << " * @param en enum value" << std::endl;
        indent(ofs, 1) << " * @return enum value converted as string" << std::endl;
        indent(ofs, 1) << " */ " << std::endl;
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
        indent(ofs, 1) << "typedef std::tuple<Enum, const char *, const char *> EnumArrayEntryType;" << std::endl;
        indent(ofs, 1) << "typedef std::array<EnumArrayEntryType, " << enumValues.size() << "> EnumArrayType;" << std::endl;
        indent(ofs, 1) << "static const EnumArrayType ENUMARRAY;" << std::endl;
        ofs << std::endl;
        indent(ofs, 1) << "static const EnumArrayEntryType& enumVal2Entry(Enum en);" << std::endl;
        ofs << std::endl;
        ofs << "protected:" << std::endl;
        indent(ofs, 1) << "static std::string enumIri(Enum en);" << std::endl;
    }

    ofs << "};" << std::endl;
    ofs << std::endl;
    leaveNameSpace(ofs);

    generateCodeProtectorEnd(ofs, genCppNameSpaceFullyQualified(), cppName);
}

void Klass::generateInterfaceDefinition() const {
    std::string cppName = "I" + genCppName();

    std::ofstream ofs;
    createFile(genCppNameSpaceInclusionPath() + "/" + cppName + ".cpp", &ofs);

    ofs << "#include <" << genCppNameSpaceInclusionPath() << "/" << cppName << ".h>" << std::endl;
    ofs << std::endl;
    addBoilerPlate(ofs);
    ofs << std::endl;
    ofs << "#include <sstream>" << std::endl;
    ofs << "#include <autordf/Exception.h>" << std::endl;
    ofs << std::endl;

    // Generate class imports
    std::set<std::shared_ptr<const Klass> > cppDeps = getClassDependencies();
    for ( const std::shared_ptr<const Klass>& cppDep : cppDeps ) {
        ofs << "#include <" << cppDep->genCppNameSpaceInclusionPath() << "/" << cppDep->genCppName() << ".h>" << std::endl;
    }
    // Include our own class
    ofs << "#include <" << genCppNameSpaceInclusionPath() << "/" << genCppName() << ".h>" << std::endl;
    ofs << std::endl;

    enterNameSpace(ofs);
    ofs << std::endl;

    if ( enumValues.size() ) {
        ofs << "const " << genCppName() << "::EnumArrayType I" << genCppName() << "::ENUMARRAY = {" << std::endl;
        for ( const RdfsEntity& enumVal : enumValues ) {
            indent(ofs, 1) << "std::make_tuple(I" <<  genCppName() << "::" << enumVal.genCppName() << ", \"" << enumVal.rdfname << "\", \"" << enumVal.genCppName() << "\")," << std::endl;
        }
        ofs << "};" << std::endl;
        ofs << std::endl;

        ofs << "const I" << genCppName() << "::EnumArrayEntryType& I" << genCppName() << "::enumVal2Entry(Enum enumVal) {" << std::endl;
        indent(ofs, 1) << "for ( auto const& enumItem: ENUMARRAY) {" << std::endl;
        indent(ofs, 2) << "if ( enumVal == std::get<0>(enumItem) ) return enumItem;" << std::endl;
        indent(ofs, 1) << "}" << std::endl;
        indent(ofs, 1) << "std::stringstream ss;" << std::endl;
        indent(ofs, 1) << "ss << \"Enum value \" << enumVal << \" is not valid for for C++ enum " << genCppName() << "\";" << std::endl;
        indent(ofs, 1) << "throw autordf::InvalidEnum(ss.str());" << std::endl;
        ofs << "};" << std::endl;

        ofs << std::endl;
        ofs << "I" << genCppName() << "::Enum I" << genCppName() << "::asEnum() const {" << std::endl;
        indent(ofs, 1) << "for ( auto const& enumItem: ENUMARRAY) {" << std::endl;
        indent(ofs, 2) << "if ( object().iri() == std::get<1>(enumItem) ) return std::get<0>(enumItem);" << std::endl;
        indent(ofs, 1) << "}" << std::endl;
        indent(ofs, 1) << "throw autordf::InvalidEnum(object().iri() + \"does not point to a valid individual for C++ enum " << genCppName() << "\");" << std::endl;
        ofs << "}" << std::endl;
        ofs << std::endl;

        ofs << "std::string I" << genCppName() << "::enumIri(Enum enumVal) {" << std::endl;
        indent(ofs, 1) << "return std::get<1>(enumVal2Entry(enumVal));" << std::endl;
        ofs << "}" << std::endl;
        ofs << std::endl;

        ofs << "std::string I" << genCppName() << "::enumString(Enum enumVal) {" << std::endl;
        indent(ofs, 1) << "return std::get<2>(enumVal2Entry(enumVal));" << std::endl;
        ofs << "}" << std::endl;
        ofs << std::endl;
    }

    ofs << "const char * " << cppName << "::TYPEIRI = \"" << rdfname << "\";" << std::endl;
    ofs << std::endl;

    for ( const std::shared_ptr<DataProperty>& prop : dataProperties) {
        prop->generateDefinition(ofs, *this);
    }
    for ( const std::shared_ptr<ObjectProperty>& prop : objectProperties) {
        prop->generateDefinition(ofs, *this);
    }
    leaveNameSpace(ofs);
}

std::set<std::shared_ptr<const Klass> > Klass::getClassDependencies() const {
    std::set<std::shared_ptr<const Klass> > deps;
    for ( const std::shared_ptr<ObjectProperty> p : objectProperties) {
        auto val = p->findClass();
        if ( val && (val->genCppName() != genCppName()) ) {
            deps.insert(val);
        }
    }
    return deps;
}

std::set<std::shared_ptr<const Klass> > Klass::getAllAncestors() const {
    std::set<std::shared_ptr<const Klass> > all;
    for ( auto ancestor = directAncestors.begin(); ancestor != directAncestors.end(); ++ancestor ) {
        all.insert(uri2Ptr[*ancestor]);
        for ( std::shared_ptr<const Klass> more : uri2Ptr[*ancestor]->getAllAncestors() ) {
            all.insert(more);
        }
    }
    return all;
}

void Klass::enterNameSpace(std::ofstream& ofs) const {
    if ( outdir != "." ) {
        ofs << "namespace " << outdir << " {" << std::endl;
    }
    ofs << "namespace " << genCppNameSpace() << " {" << std::endl;
}

void Klass::leaveNameSpace(std::ofstream& ofs) const {
    ofs << "}" << std::endl;
    if ( outdir != "." ) {
        ofs << "}" << std::endl;
    }
}
}
}


