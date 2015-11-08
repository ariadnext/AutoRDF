#include "Klass.h"

#include "autordf/ontology/Ontology.h"

#include <fstream>

#include "Utils.h"

namespace autordf {
namespace codegen {

Klass Klass::uri2Klass(const std::string& uri) const {
    return Klass(_decorated.ontology().findClass(uri));
}

void Klass::generateDeclaration() const {
    std::string cppName = _decorated.prettyIRIName();
    std::ofstream ofs;
    createFile(genCppNameSpaceInclusionPath() + "/" + cppName + ".h", &ofs);

    generateCodeProtectorBegin(ofs, genCppNameSpaceForGuard(), cppName);

    ofs << "#include <set>" << std::endl;
    ofs << "#include <autordf/Object.h>" << std::endl;
    ofs << "#include <" << genCppNameSpaceInclusionPath() << "/I" << cppName << ".h>" << std::endl;
    for ( auto const& ancestor: _decorated.ancestors() ) {
        ofs << "#include <" << uri2Klass(ancestor->rdfname()).genCppNameSpaceInclusionPath() << "/I" <<
                ancestor->prettyIRIName() << ".h>" << std::endl;
    }
    ofs << std::endl;

    enterNameSpace(ofs);
    ofs << std::endl;

    generateComment(ofs, 0);
    ofs << "class " << cppName << ": public autordf::Object";
    for ( auto const& ancestor: _decorated.ancestors() ) {
        ofs << ", public " << uri2Klass(ancestor->rdfname()).genCppNameSpaceFullyQualified() << "::I" <<
                ancestor->prettyIRIName();
    }
    ofs << ", public I" << cppName << " {" << std::endl;
    ofs << "public:" << std::endl;
    ofs << std::endl;
    if ( !_decorated.oneOfValues().size() ) {
        indent(ofs, 1) << "/**" << std::endl;
        indent(ofs, 1) << " * @brief Creates new object, to given iri." << std::endl;
        indent(ofs, 1) << " * " << std::endl;
        indent(ofs, 1) << " * If iri empty, creates an anonymous (aka blank) object" << std::endl;
        indent(ofs, 1) << " */" << std::endl;
        indent(ofs, 1) << cppName << "(const std::string& iri = \"\");" << std::endl;
    } else {
        indent(ofs, 1) << "/**" << std::endl;
        indent(ofs, 1) << " * @brief Load enum from RDF model, from given C++ Type enum." << std::endl;
        indent(ofs, 1) << " * " << std::endl;
        indent(ofs, 1) << " * This applies only to classes defines using the owl:oneOf paradigm" << std::endl;
        indent(ofs, 1) << " */" << std::endl;
        indent(ofs, 1) << cppName << "(I" << cppName << "::Enum enumVal);" << std::endl;
    }
    ofs << std::endl;
    indent(ofs, 1) << "/**" << std::endl;
    indent(ofs, 1) << " * @brief Build us using the same underlying resource as the other object" << std::endl;
    indent(ofs, 1) << " */" << std::endl;
    indent(ofs, 1) << cppName << "(const Object& other);" << std::endl;
    ofs << std::endl;
    indent(ofs, 1) << "/**" << std::endl;
    indent(ofs, 1) << " * @brief Returns the list of all objects of this kind in the store" << std::endl;
    indent(ofs, 1) << " */" << std::endl;
    indent(ofs, 1) << "static std::list<" << cppName << "> find();" << std::endl;
    ofs << std::endl;
    startInternal(ofs, 1);
    indent(ofs, 1) << "/**" << std::endl;
    indent(ofs, 1) << " * @brief Internal: returns full list of ancestors we have" << std::endl;
    indent(ofs, 1) << " **/" << std::endl;
    indent(ofs, 1) << "static std::set<std::string> ancestorsRdfTypeIRI();" << std::endl;
    stopInternal(ofs, 1);
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
    std::string cppName = _decorated.prettyIRIName();
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
    if ( !_decorated.oneOfValues().size() ) {
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
    for ( const std::shared_ptr<const ontology::Klass>& ancestor : _decorated.getAllAncestors() ) {
        indent(ofs, 3) << Klass(*ancestor.get()).genCppNameSpaceFullyQualified() << "::I" <<
                ancestor->prettyIRIName() << "::TYPEIRI," << std::endl;
    }
    indent(ofs, 2) <<         "});" << std::endl;
    ofs << "}" << std::endl;
    ofs << std::endl;
    leaveNameSpace(ofs);
    ofs << std::endl;
}

void Klass::generateInterfaceDeclaration() const {
    std::string cppName = "I" + _decorated.prettyIRIName();
    std::ofstream ofs;
    createFile(genCppNameSpaceInclusionPath() + "/" + cppName + ".h", &ofs);

    generateCodeProtectorBegin(ofs, genCppNameSpaceForGuard(), cppName);

    if ( _decorated.oneOfValues().size() ) {
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
        ofs << "class " << cppClassDep->decorated().prettyIRIName() << ";" << std::endl;
        cppClassDep->leaveNameSpace(ofs);
    }
    // Add forward declaration for our own class
    enterNameSpace(ofs);
    ofs << "class " << _decorated.prettyIRIName() << ";" << std::endl;
    leaveNameSpace(ofs);
    ofs << std::endl;

    enterNameSpace(ofs);
    ofs << std::endl;

    generateComment(ofs, 0);
    ofs << "class " << cppName << " ";
    ofs << " {" << std::endl;
    ofs << "public:" << std::endl;
    startInternal(ofs, 1);
    indent(ofs, 1) << "/**" << std::endl;
    indent(ofs, 1) << " * IRI for rdfs type associated with this C++ Interface class" << std::endl;
    indent(ofs, 1) << " */ " << std::endl;
    indent(ofs, 1) << "static const char* TYPEIRI;" << std::endl;
    stopInternal(ofs, 1);
    ofs << std::endl;

    if ( _decorated.oneOfValues().size() ) {
        indent(ofs, 1) << "/**" << std::endl;
        indent(ofs, 1) << " * @brief C++ Enum values mapping the owl instances restrictions for this class " << std::endl;
        indent(ofs, 1) << " */ " << std::endl;
        indent(ofs, 1) << "enum Enum {" << std::endl;
        for ( const RdfsEntity& enumVal : _decorated.oneOfValues() ) {
            enumVal.generateComment(ofs, 2);
            indent(ofs, 2) << enumVal.decorated().prettyIRIName() << "," << std::endl;
        }
        indent(ofs, 1) << "};" << std::endl;
        ofs << std::endl;

        indent(ofs, 1) << "/** " << std::endl;
        indent(ofs, 1) << " * @brief Returns the current object as an Enum " << std::endl;
        indent(ofs, 1) << " * " << std::endl;
        indent(ofs, 1) << " * This object is expected to be one the instance allowed by the owl:oneOf definition." << std::endl;
        indent(ofs, 1) << " * @return enum value" << std::endl;
        indent(ofs, 1) << " * @throw InvalidEnum if the object we try to convert is not one of the instances defined by owl:oneOf" << std::endl;
        indent(ofs, 1) << " */ " << std::endl;
        indent(ofs, 1) << "Enum asEnum() const;" << std::endl;
        ofs << std::endl;
        indent(ofs, 1) << "/** " << std::endl;
        indent(ofs, 1) << " * @brief Converts an enum value to a pretty string " << std::endl;
        indent(ofs, 1) << " * " << std::endl;
        indent(ofs, 1) << " * @param en enum value" << std::endl;
        indent(ofs, 1) << " * @return enum value converted as string" << std::endl;
        indent(ofs, 1) << " */ " << std::endl;
        indent(ofs, 1) << "static std::string enumString(Enum en);" << std::endl;
    }
    ofs << std::endl;
    for ( const std::shared_ptr<ontology::DataProperty>& prop : _decorated.dataProperties()) {
        DataProperty(*prop.get()).generateDeclaration(ofs, _decorated);
    }

    for ( const std::shared_ptr<ontology::ObjectProperty>& prop : _decorated.objectProperties()) {
        ObjectProperty(*prop.get()).generateDeclaration(ofs, _decorated);
    }
    ofs << std::endl;

    ofs << "private:" << std::endl;
    indent(ofs, 1) << "virtual autordf::Object& object() = 0;" << std::endl;
    indent(ofs, 1) << "virtual const autordf::Object& object() const = 0;" << std::endl;
    ofs << std::endl;
    if ( _decorated.oneOfValues().size() ) {
        indent(ofs, 1) << "typedef std::tuple<Enum, const char *, const char *> EnumArrayEntryType;" << std::endl;
        indent(ofs, 1) << "typedef std::array<EnumArrayEntryType, " << _decorated.oneOfValues().size() << "> EnumArrayType;" << std::endl;
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
    std::string cppName = "I" + _decorated.prettyIRIName();

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
        ofs << "#include <" << cppDep->genCppNameSpaceInclusionPath() << "/" << cppDep->decorated().prettyIRIName() << ".h>" << std::endl;
    }
    // Include our own class
    ofs << "#include <" << genCppNameSpaceInclusionPath() << "/" << _decorated.prettyIRIName() << ".h>" << std::endl;
    ofs << std::endl;

    enterNameSpace(ofs);
    ofs << std::endl;

    if ( _decorated.oneOfValues().size() ) {
        ofs << "const " << _decorated.prettyIRIName() << "::EnumArrayType I" << _decorated.prettyIRIName() << "::ENUMARRAY = {" << std::endl;
        for ( const ontology::RdfsEntity& v : _decorated.oneOfValues() ) {
            RdfsEntity enumVal(v);
            indent(ofs, 1) << "std::make_tuple(I" << _decorated.prettyIRIName() << "::" << enumVal.decorated().prettyIRIName() << ", \"" << v.rdfname() << "\", \"" <<
                    enumVal.decorated().prettyIRIName() << "\")," << std::endl;
        }
        ofs << "};" << std::endl;
        ofs << std::endl;

        ofs << "const I" << _decorated.prettyIRIName() << "::EnumArrayEntryType& I" << _decorated.prettyIRIName() << "::enumVal2Entry(Enum enumVal) {" << std::endl;
        indent(ofs, 1) << "for ( auto const& enumItem: ENUMARRAY) {" << std::endl;
        indent(ofs, 2) << "if ( enumVal == std::get<0>(enumItem) ) return enumItem;" << std::endl;
        indent(ofs, 1) << "}" << std::endl;
        indent(ofs, 1) << "std::stringstream ss;" << std::endl;
        indent(ofs, 1) << "ss << \"Enum value \" << enumVal << \" is not valid for for C++ enum " << _decorated.prettyIRIName() << "\";" << std::endl;
        indent(ofs, 1) << "throw autordf::InvalidEnum(ss.str());" << std::endl;
        ofs << "};" << std::endl;

        ofs << std::endl;
        ofs << "I" << _decorated.prettyIRIName() << "::Enum I" << _decorated.prettyIRIName() << "::asEnum() const {" << std::endl;
        indent(ofs, 1) << "for ( auto const& enumItem: ENUMARRAY) {" << std::endl;
        indent(ofs, 2) << "if ( object().iri() == std::get<1>(enumItem) ) return std::get<0>(enumItem);" << std::endl;
        indent(ofs, 1) << "}" << std::endl;
        indent(ofs, 1) << "throw autordf::InvalidEnum(object().iri() + \"does not point to a valid individual for C++ enum " <<
                _decorated.prettyIRIName() << "\");" << std::endl;
        ofs << "}" << std::endl;
        ofs << std::endl;

        ofs << "std::string I" << _decorated.prettyIRIName() << "::enumIri(Enum enumVal) {" << std::endl;
        indent(ofs, 1) << "return std::get<1>(enumVal2Entry(enumVal));" << std::endl;
        ofs << "}" << std::endl;
        ofs << std::endl;

        ofs << "std::string I" << _decorated.prettyIRIName() << "::enumString(Enum enumVal) {" << std::endl;
        indent(ofs, 1) << "return std::get<2>(enumVal2Entry(enumVal));" << std::endl;
        ofs << "}" << std::endl;
        ofs << std::endl;
    }

    ofs << "const char * " << cppName << "::TYPEIRI = \"" << _decorated.rdfname() << "\";" << std::endl;
    ofs << std::endl;

    for ( const std::shared_ptr<ontology::DataProperty>& prop : _decorated.dataProperties()) {
        DataProperty(*prop.get()).generateDefinition(ofs, _decorated);
    }
    for ( const std::shared_ptr<ontology::ObjectProperty>& prop : _decorated.objectProperties()) {
        ObjectProperty(*prop.get()).generateDefinition(ofs, _decorated);
    }
    leaveNameSpace(ofs);
}

std::set<std::shared_ptr<const Klass> > Klass::getClassDependencies() const {
    std::set<std::shared_ptr<const Klass> > deps;
    for ( const std::shared_ptr<ontology::ObjectProperty> p : _decorated.objectProperties()) {
        auto val = p->findClass();
        if ( val && (val->prettyIRIName() != _decorated.prettyIRIName()) ) {
            deps.insert(std::shared_ptr<Klass>(new Klass(*val.get())));
        }
    }
    return deps;
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


