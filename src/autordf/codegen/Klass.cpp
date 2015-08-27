#include "Klass.h"

#include <fstream>

#include "Utils.h"

namespace autordf {
namespace codegen {

std::map <std::string, std::shared_ptr<Klass>> Klass::uri2Ptr;

std::string Klass::outdir = ".";

void Klass::generateDeclaration() const {
    std::string cppName = genCppName();
    std::ofstream ofs;
    createFile(outdir + "/" + genCppNameSpace() + "/" + cppName + ".h", &ofs);

    generateCodeProtectorBegin(ofs, genCppNameSpace(), cppName);

    ofs << "#include <set>" << std::endl;
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

    indent(ofs, 1) << "Object& object() { return *this; }" << std::endl;
    indent(ofs, 1) << "const Object& object() const { return *this; }" << std::endl;
    ofs << "};" << std::endl;
    ofs << std::endl;
    ofs << "}" << std::endl;

    generateCodeProtectorEnd(ofs, genCppNameSpace(), cppName);
}

void Klass::generateDefinition() const {
    std::string cppName = genCppName();
    std::string cppNameSpace = genCppNameSpace();
    std::ofstream ofs;
    createFile(outdir + "/" + cppNameSpace + "/" + cppName + ".cpp", &ofs);

    ofs << "#include <" << cppNameSpace << "/" << cppName << ".h>" << std::endl;
    ofs << std::endl;
    addBoilerPlate(ofs);
    ofs << std::endl;

    ofs << "#include \"RdfTypeInfo.h\"" << std::endl;
    ofs << std::endl;

    ofs << "namespace " << cppNameSpace << " {" << std::endl;
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
        indent(ofs, 3) << ancestor->genCppNameSpace() << "::I" << ancestor->genCppName() << "::TYPEIRI," << std::endl;
    }
    indent(ofs, 2) <<         "});" << std::endl;
    ofs << "}" << std::endl;
    ofs << std::endl;
    ofs << "}" << std::endl;
    ofs << std::endl;
}

void Klass::generateInterfaceDeclaration() const {
    std::string cppName = "I" + genCppName();
    std::ofstream ofs;
    createFile(outdir + "/" + genCppNameSpace() + "/" + cppName + ".h", &ofs);

    generateCodeProtectorBegin(ofs, genCppNameSpace(), cppName);

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
            indent(ofs, 2) << codegen::genCppName(en) << "," << std::endl;
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
    ofs << "}" << std::endl;

    generateCodeProtectorEnd(ofs, genCppNameSpace(), cppName);
}

void Klass::generateInterfaceDefinition() const {
    std::string cppName = "I" + genCppName();
    std::string cppNameSpace = genCppNameSpace();

    std::ofstream ofs;
    createFile(outdir + "/" + cppNameSpace + "/" + cppName + ".cpp", &ofs);

    ofs << "#include <" << cppNameSpace << "/" << cppName << ".h>" << std::endl;
    ofs << std::endl;
    addBoilerPlate(ofs);
    ofs << std::endl;
    ofs << "#include <sstream>" << std::endl;
    ofs << "#include <autordf/Exception.h>" << std::endl;
    ofs << std::endl;

    // Generate class imports
    std::set<std::shared_ptr<const Klass> > cppDeps = getClassDependencies();
    for ( const std::shared_ptr<const Klass>& cppDep : cppDeps ) {
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
            indent(ofs, 1) << "std::make_tuple(I" <<  genCppName() << "::" << codegen::genCppName(en) << ", \"" << en << "\", \"" << codegen::genCppName(en) << "\")," << std::endl;
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

    ofs << "const std::string& " << cppName << "::TYPEIRI = \"" << rdfname << "\";" << std::endl;
    ofs << std::endl;

    for ( const std::shared_ptr<DataProperty>& prop : dataProperties) {
        prop->generateDefinition(ofs, *this);
    }
    for ( const std::shared_ptr<ObjectProperty>& prop : objectProperties) {
        prop->generateDefinition(ofs, *this);
    }
    ofs << "}" << std::endl;
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

}
}


