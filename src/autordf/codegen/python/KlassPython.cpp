#include "KlassPython.h"
#include "DataPropertyPython.h"
#include "ObjectPropertyPython.h"
#include "autordf/ontology/Ontology.h"

#include <fstream>
#include <iostream>
#include <iterator>

#include "UtilsPython.h"
#include <autordf/codegen/UtilsCommon.h>

namespace autordf {
namespace codegen {

KlassPython KlassPython::uri2Klass(const std::string& uri) const {
    return KlassPython(*_decorated.ontology()->findClass(uri));
}

void KlassPython::generate() const {
    std::string name = _decorated.prettyIRIName();
    std::string moduleName = genModuleFullyQualified();
    std::ofstream ofs;
    createFile(genModuleInclusionPath() + "/" + name + ".py", &ofs);

    startInternal(ofs);
    ofs << "import autordf_py" << std::endl;
    ofs << "from " << genModuleFullyQualified() << ".I" << name << " import I" << name << std::endl;
    for ( auto const& ancestor: _decorated.getAllAncestors() ) {
        ofs << "from " << uri2Klass(ancestor->rdfname()).genModuleFullyQualified() << ".I" << ancestor->prettyIRIName() <<
            " import I" << ancestor->prettyIRIName() << std::endl;
    }
    ofs << std::endl;
    addBoilerPlate(ofs);
    ofs << std::endl;
    generateComment(ofs, 0);
    ofs << "class " << name << "(";
    for ( auto const& ancestor: _decorated.getAllAncestors() ) {
        ofs << "I" << ancestor->prettyIRIName() << ", ";
    }
    ofs << "I" << name << ", autordf_py.Object):" << std::endl;

    //We use kwargs to be able to switch constructors
    // TODO potentially replace with dispatchmethods if we go for typing?

    bool enumed = !_decorated.oneOfValues().empty();
    indent(ofs, 1) << R"(""")" << std::endl;
    indent(ofs, 1) << "@brief Creates new object" << std::endl;
    if (enumed) {
        indent(ofs, 1) << "@param enumVal if set, use the iri of the enum value to build the object" << std::endl;
        indent(ofs, 1) << "This applies only to classes defines using the owl:oneOf paradigm" << std::endl;
    }
    indent(ofs, 1) << "@param other if set, build the new object using the same underlying resource as the other object" << std::endl;
    indent(ofs, 1) << "@param iri if iri empty, creates an anonymous (aka blank) object" << std::endl;
    indent(ofs, 1) << R"(""")" << std::endl;
    indent(ofs, 1) << R"(def __init__(self, *args, **kwargs):)" << std::endl;
    // Constructor if we have an enum value
    if (enumed) {
        indent(ofs, 2) << R"(if "enumVal" in kwargs:)" << std::endl;
        indent(ofs, 3) << R"(autordf_py.Object.__init__(self, enumIri(kwargs["enumVal"])))"  << std::endl;
    }
    indent(ofs, 2) << (enumed ? "elif" : "if") << R"( "other" in kwargs:)" << std::endl;
    indent(ofs, 3) << R"(autordf_py.Object.__init__(self, kwargs["other"]))" << std::endl;
    indent(ofs, 2) << "else:" << std::endl;
    indent(ofs, 3) << R"#(autordf_py.Object.__init__(self, kwargs.get("iri", ""), I)#" << name << ".TYPEIRI)" << std::endl;
    ofs << std::endl;

    indent(ofs, 1) << R"(""")" << std::endl;
    indent(ofs, 1) << "@brief Clone the object, to given iri" << std::endl;
    indent(ofs, 1) << "Object is copied by duplicating all its properties values. " << std::endl;
    indent(ofs, 1) << "@param iri if empty, creates an anonymous (aka blank) object." << std::endl;
    indent(ofs, 1) << R"(""")" << std::endl;
    indent(ofs, 1) << R"(def clone(self, iri = ""):)" << std::endl;
    indent(ofs, 2) << "return " << name << "(other=autordf_py.Object.clone(self, iri))" << std::endl;
    ofs << std::endl;
    indent(ofs, 1) << R"(""")" << std::endl;
    indent(ofs, 1) << "@brief This method " << genModuleFullyQualified() << "." << name << ".find returns all resources of type " << _decorated.rdfname() << std::endl;
    indent(ofs, 1) << R"(""")" << std::endl;
    indent(ofs, 1) << "@staticmethod" << std::endl;
    indent(ofs, 1) << "def find():" << std::endl;
    indent(ofs, 2) << "return [" << name << "(other=found) for found in autordf_py.Object.findByType(I" << name << ".TYPEIRI)]" << std::endl;
    ofs << std::endl;
    stopInternal(ofs);
}

void KlassPython::generateInterface() const {
    std::string name = "I" + _decorated.prettyIRIName();

    std::ofstream ofs;
    createFile(genModuleInclusionPath() + "/" + name + ".py", &ofs);

    startInternal(ofs);
    addBoilerPlate(ofs);
    ofs << std::endl;
    ofs << "import autordf_py" << std::endl;

    if (!_decorated.oneOfValues().empty()) {
        ofs << "from enum import Enum" << std::endl;
    }
    ofs << std::endl;

    // Generate class imports
    std::set<KlassPython> cppDeps = getClassDependencies();
    for (auto cppDep : cppDeps ) {
        ofs << "import " << cppDep.genModuleFullyQualified() << "." << cppDep.decorated().prettyIRIName() << std::endl;
    }
    ofs << std::endl;

    ofs << "class " << name << "(autordf_py.Object):" << std::endl;
    ofs << std::endl;

    if (!_decorated.oneOfValues().empty()) {
        indent(ofs, 1) << R"(""")" << std::endl;
        indent(ofs, 1) << "@brief Enum values mapping the owl instances restrictions for this class" << std::endl;
        indent(ofs, 1) << R"(""")" << std::endl;
        indent(ofs, 1) << "class InternalEnum(Enum):" << std::endl;

        int index = 1;
        for ( const RdfsEntityPython enumVal : _decorated.oneOfValues() ) {
            enumVal.generateComment(ofs, 2);
            indent(ofs, 2) << enumVal.decorated().prettyIRIName() << " = " << index << std::endl;
            index++;
        }
        indent(ofs, 1) <<  "ENUMARRAY = [" << std::endl;
        for ( const ontology::RdfsEntity& v : _decorated.oneOfValues() ) {
            RdfsEntityPython enumVal(v);
            indent(ofs, 2) << "(InternalEnum." << enumVal.decorated().prettyIRIName() << ", \"" << v.rdfname() << "\", \"" <<
                    enumVal.decorated().prettyIRIName() << "\")," << std::endl;
        }
        indent(ofs,1) << "]" << std::endl;
        ofs << std::endl;

        indent(ofs, 1) << R"(""")" << std::endl;
        indent(ofs, 1) << "@brief Returns the associated mappings (rdf name, pretty name) to an enum value" << std::endl;
        indent(ofs, 1) << R"(""")" << std::endl;
        indent(ofs, 1) << "@classmethod" << std::endl;
        indent(ofs, 1) << "def enumVal2Entry(cls, enumVal):" << std::endl;
        indent(ofs, 2) << "for enumItem in cls.ENUMARRAY:" << std::endl;
        indent(ofs, 3) << "if enumVal == enumItem[0]:" << std::endl;
        indent(ofs, 4) << "return enumItem" << std::endl;
        indent(ofs, 2) << R"(exceptString = "Enum value %1 is not valid for python enum )" << _decorated.prettyIRIName() << "\".format(enumVal)" << std::endl;
        indent(ofs, 2) << "raise autordf_py.InvalidEnum(exceptString)" << std::endl;
        ofs << std::endl;
        indent(ofs, 1) << R"(""")" << std::endl;
        indent(ofs, 1) << "@brief Returns the current object as an Enum " << std::endl;
        ofs << std::endl;
        indent(ofs, 1) << "This object is expected to be one the instance allowed by the owl:oneOf definition." << std::endl;
        indent(ofs, 1) << "@return enum value" << std::endl;
        indent(ofs, 1) << "@throw InvalidEnum if the object we try to convert is not one of the instances defined by owl:oneOf" << std::endl;
        indent(ofs, 1) << R"(""")" << std::endl;
        indent(ofs, 1) << "def asEnum(self):" << std::endl;
        indent(ofs, 2) << "for enumItem in self.ENUMARRAY:" << std::endl;
        indent(ofs, 3) << "if self.iri() == enumItem[1]:" << std::endl;
        indent(ofs, 4) << "return enumItem[0]" << std::endl;
        indent(ofs, 2) << "raise autordf_py.InvalidEnum(self.iri() + \" is not a valid individual for owl:oneOf type " <<
                _decorated.prettyIRIName() << "\")" << std::endl;
        ofs << std::endl;

        indent(ofs, 1) << "@classmethod" << std::endl;
        indent(ofs, 1) << "def enumIri(cls, enumVal):" << std::endl;
        indent(ofs, 2) << "return cls.enumVal2Entry(enumVal)[1]" << std::endl;
        ofs << std::endl;
        indent(ofs, 1) << R"(""")" << std::endl;
        indent(ofs, 1) << "@brief Converts an enum value to a pretty string " << std::endl;
        ofs << std::endl;
        indent(ofs, 1) << "@param enumVal enum value" << std::endl;
        indent(ofs, 1) << "@return enum value converted as string" << std::endl;
        indent(ofs, 1) << R"(""")" << std::endl;
        indent(ofs, 1) << "@classmethod" << std::endl;
        indent(ofs, 1) << "def enumString(cls, enumVal):" << std::endl;
        indent(ofs, 2) << "return cls.enumVal2Entry(enumVal)[2]" << std::endl;
        ofs << std::endl;
        indent(ofs, 1) << R"(""")" << std::endl;
        indent(ofs, 1) << "@brief Converts an enum as a string to an enum " << std::endl;
        ofs << std::endl;
        indent(ofs, 1) << "@param enumString enum as string" << std::endl;
        indent(ofs, 1) << "@return enum value" << std::endl;
        indent(ofs, 1) << "@throw InvalidEnum if the string we try to convert is not one of the instances defined by owl:oneOf" << std::endl;
        indent(ofs, 1) << R"(""")" << std::endl;

        indent(ofs, 1) << "@classmethod" << std::endl;
        indent(ofs, 1) << "def enumFromString(cls, enumString):" << std::endl;
        indent(ofs, 2) << "for enumItem in cls.ENUMARRAY:" << std::endl;
        indent(ofs, 3) << "if enumString == enumItem[2]:" << std::endl;
        indent(ofs, 4) << "return enumItem[0]" << std::endl;
        indent(ofs, 2) << "raise autordf_py.InvalidEnum(enumString + \" is not a valid individual for owl:oneOf type " <<
                       _decorated.prettyIRIName() << "\")" << std::endl;
        ofs << std::endl;

        indent(ofs, 1) << R"(""")" << std::endl;
        indent(ofs, 1) << "@brief Converts current enum value to a pretty string " << std::endl;
        ofs << std::endl;
        indent(ofs, 1) << "@return current enum value converted as string" << std::endl;
        indent(ofs, 1) << R"(""")" << std::endl;
        ofs << std::endl;
        indent(ofs, 1) << "def enumStringFromSelf(self):" << std::endl;
        indent(ofs, 2) << "return self.enumString(self.asEnum())" << std::endl;
    }
    indent(ofs, 1) << R"("""This type )" << genModuleFullyQualified() << "." << name << " has IRI " << _decorated.rdfname() << R"(""")" << std::endl;
    indent(ofs, 1) << "TYPEIRI = \"" << _decorated.rdfname() << "\"" << std::endl;
    ofs << std::endl;

    for ( const std::shared_ptr<ontology::AnnotationProperty>& key : _decorated.annotationKeys()) {
        DataPropertyPython(*key).generateKey(ofs, _decorated);
    }

    for ( const std::shared_ptr<ontology::DataProperty>& key : _decorated.dataKeys()) {
        DataPropertyPython(*key).generateKey(ofs, _decorated);
    }

    for ( const std::shared_ptr<ontology::ObjectProperty>& key : _decorated.objectKeys()) {
        ObjectPropertyPython(*key).generateKey(ofs, _decorated);
    }

    for ( const std::shared_ptr<ontology::AnnotationProperty>& prop : _decorated.annotationProperties()) {
        DataPropertyPython(*prop).generate(ofs, _decorated);
    }
    for ( const std::shared_ptr<ontology::DataProperty>& prop : _decorated.dataProperties()) {
        DataPropertyPython(*prop).generate(ofs, _decorated);
    }
    for ( const std::shared_ptr<ontology::ObjectProperty>& prop : _decorated.objectProperties()) {
        ObjectPropertyPython(*prop).generate(ofs, _decorated);
    }
    stopInternal(ofs);
}

std::set<KlassPython> KlassPython::getClassDependencies() const {
    std::set<KlassPython> deps;
    std::set<std::shared_ptr<const ontology::ObjectProperty> > objects;

    std::copy(_decorated.objectProperties().begin(), _decorated.objectProperties().end(), std::inserter(objects, objects.begin()));
    std::copy(_decorated.objectKeys().begin(), _decorated.objectKeys().end(), std::inserter(objects, objects.begin()));

    for ( const std::shared_ptr<const ontology::ObjectProperty>& p : objects) {
        auto val = p->findClass(&_decorated);
        if ( val ) {
            if ( val->prettyIRIName() != _decorated.prettyIRIName() ) {
                deps.emplace(*val);
            }
        } else {
            deps.emplace(*(_decorated.ontology()->findClass(autordf::ontology::Ontology::OWL_NS + "Thing")));
        }
    }

    return deps;
}
}
}


