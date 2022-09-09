#include <autordf/ontology/Ontology.h>
#include "ObjectPropertyPython.h"

#include "KlassPython.h"
#include "UtilsPython.h"

namespace autordf {
namespace codegen {

void ObjectPropertyPython::generate(std::ostream& ofs, const KlassPython& onClass) const {
    auto propertyClass = effectiveClass(onClass);
    indent(ofs, 1) << _decorated.prettyIRIName() << "ObjectPropertyIri = \"" << _decorated.rdfname() << "\"" << std::endl;
    ofs << std::endl;

    if ( _decorated.maxCardinality(onClass.decorated()) <= 1 ) {
        if ( _decorated.minCardinality(onClass.decorated()) > 0 ) {
            std::string methodName = "get" + _decorated.prettyIRIName(true);
            generatePropertyComment(ofs, onClass, methodName,  1,
                                    "@return the mandatory instance.\n"
                                    "@throw PropertyNotFound if object reference is not set", &propertyClass);
            indent(ofs, 1) << "def " << methodName << "(self):" << std::endl;
            indent(ofs, 2) << "return " << propertyClass.genNameWithModule() << "(other=self.getObject(\"" << _decorated.rdfname() << "\"))" << std::endl;
            ofs << std::endl;
        } else {
            std::string methodName = "get" + _decorated.prettyIRIName(true) + "Optional";
            generatePropertyComment(ofs, onClass, methodName,  1,
                                    "@return the object instance if it is set, or None if it is not set.", &propertyClass);
            indent(ofs, 1) << "def " << methodName << "(self):" << std::endl;
            indent(ofs, 2) << "opt = self.getOptionalObject(\"" << _decorated.rdfname() << "\")" << std::endl;
            indent(ofs, 2) << "return None if opt == None else " << propertyClass.genNameWithModule() << "(other=opt)" << std::endl;
            ofs << std::endl;
        }
    }
    if ( _decorated.maxCardinality(onClass.decorated()) > 1 ) {
        std::string methodName = "get" + _decorated.prettyIRIName(true) + "List";
        generatePropertyComment(ofs, onClass, methodName,  1,
                                "@return the list typed objects.  List can be empty if not values are set in database", &propertyClass);
        indent(ofs, 1) << "def " << methodName << "(self):" << std::endl;
        indent(ofs, 2) << "return [" << propertyClass.genNameWithModule() << "(other=elem) for elem in self.getObjectList(\"" <<  _decorated.rdfname() << "\", " << orderedBoolValue() << ")]" << std::endl;
        ofs << std::endl;
        generateSetterForMany(ofs, onClass);
    }
    ofs << std::endl;
    generateSetterForOne(ofs, onClass);
    if ( _decorated.minCardinality(onClass.decorated()) != _decorated.maxCardinality(onClass.decorated()) ) {
        ofs << std::endl;
        generateRemover(ofs, onClass);
    }
    ofs << std::endl;
}

void ObjectPropertyPython::generateKey(std::ostream& ofs, const KlassPython& onClass) const {
    auto propertyClass = effectiveClass(onClass);
    std::string currentClassName = onClass.decorated().prettyIRIName();

    indent(ofs, 1) << R"(""")" << std::endl;
    indent(ofs, 1) << "@brief This method " << genModuleFullyQualified() << "." << currentClassName << ".findBy" << _decorated.prettyIRIName(true) << " returns the only instance of " + onClass.decorated().rdfname() + " with property " + _decorated.rdfname() +  " set to given object." << std::endl << std::endl;
    indent(ofs, 1) << "@param key value that uniquely identifies the expected object" << std::endl << std::endl;
    indent(ofs, 1) << "@throw DuplicateObject if more than one object have the same property value" << std::endl;
    indent(ofs, 1) << "@throw ObjectNotFound if no object has given property with value" << std::endl;
    indent(ofs, 1) << R"(""")" << std::endl;

    indent(ofs, 1) << "@staticmethod" << std::endl;
    indent(ofs, 1) << "def findBy" << _decorated.prettyIRIName(true) << "(key):" << std::endl;
    indent(ofs, 2) << "return " << currentClassName << "(other=autordf_py.Object.findByKey(\"" << _decorated.rdfname() << "\", key))" << std::endl;
    ofs << std::endl;
}

void ObjectPropertyPython::generateSetterForOne(std::ostream& ofs, const KlassPython& onClass) const {
    auto propertyClass = effectiveClass(onClass);
    std::string methodName = "set" + _decorated.prettyIRIName(true);
    generatePropertyComment(ofs, onClass, methodName,  1,
                            "Sets the mandatory value for this property.\n"
                            "@param value value to set for this property, removing all other values", &propertyClass);

    indent(ofs, 1) << "def " << methodName << "(self, value):" << std::endl;
    indent(ofs, 2) << "self.setObject(\"" << _decorated.rdfname() << "\", value)" << std::endl;
    indent(ofs, 2) << "return self" << std::endl;
    ofs << std::endl;
}

void ObjectPropertyPython::generateSetterForMany(std::ostream& ofs, const KlassPython& onClass) const {
    auto propertyClass = effectiveClass(onClass);
    std::string methodName = "set" + _decorated.prettyIRIName(true) + "List";
    generatePropertyComment(ofs, onClass, methodName, 1,
                            "Sets the values for this property.\n"
                            "@param values values to set for this property", &propertyClass);

    indent(ofs, 1) << "def " << methodName << "(self, values):" << std::endl;
    indent(ofs, 2) << "self.setObjectList(\"" <<  _decorated.rdfname() << "\", values, " << orderedBoolValue() << ")" << std::endl;
    indent(ofs, 2) << "return self" << std::endl;
    ofs << std::endl;

    methodName = "add" + _decorated.prettyIRIName(true);
    generatePropertyComment(ofs, onClass, methodName, 1,
                            "Adds a value for this property.\n"
                            "@param value value to set for this property, removing all other values", &propertyClass);
    indent(ofs, 1) << "def " << methodName << "(self, value):" << std::endl;
    indent(ofs, 2) << "self.addObject(\"" << _decorated.rdfname() << "\", value, " << orderedBoolValue() << ")" << std::endl;
    indent(ofs, 2) << "return self" << std::endl;
    ofs << std::endl;
}

KlassPython ObjectPropertyPython::effectiveClass(const KlassPython& onClass) const {
    std::shared_ptr<ontology::Klass> kls = _decorated.findClass(&onClass.decorated());
    if ( kls ) {
        return *kls;
    } else {
        return *_decorated.ontology()->findClass(autordf::ontology::Ontology::OWL_NS + "Thing");
    }
}

void ObjectPropertyPython::generateRemover(std::ostream& ofs, const KlassPython& onClass) const {
    auto propertyClass = effectiveClass(onClass);
    std::string methodName = "remove" + _decorated.prettyIRIName(true);
    generatePropertyComment(ofs, onClass, methodName,  1,
                            "Remove a value for this property.\n"
                            "@param obj value to remove for this property.\n"
                            "@throw PropertyNotFound if propertyIRI has not obj as value", &propertyClass);
    indent(ofs, 1) << "def " << methodName << "(self, value):" << std::endl;
    indent(ofs, 2) << "self.removeObject(\"" << _decorated.rdfname() << "\", value)" << std::endl;
    indent(ofs, 2) << "return self" << std::endl;
    ofs << std::endl;
}

std::string ObjectPropertyPython::orderedBoolValue() const {
    return _decorated.ordered() ? "True" : "False";
}

}
}