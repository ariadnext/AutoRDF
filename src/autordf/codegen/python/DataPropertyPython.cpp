#include "DataPropertyPython.h"

#include <autordf/cvt/Cvt.h>

#include "KlassPython.h"
#include "UtilsPython.h"

namespace autordf {
namespace codegen {

void DataPropertyPython::generate(std::ostream& ofs, const KlassPython& onClass) const {
    indent(ofs, 1) << R"("""Full iri for )" <<  _decorated.rdfname().prettyName() << R"( data property.""")" << std::endl;
    indent(ofs, 1) << _decorated.prettyIRIName() << "DataPropertyIri = \"" << _decorated.rdfname() << "\"" << std::endl;
    ofs << std::endl;

    if ( _decorated.maxCardinality(onClass.decorated()) <= 1 ) {
        if ( _decorated.minCardinality(onClass.decorated()) > 0 ) {
            generateGetterForOneMandatory(ofs, onClass);
        } else {
            generateGetterForOneOptional(ofs, onClass);
        }
        ofs << std::endl;
        generateSetterForOne(ofs, onClass);
    }
    if ( _decorated.maxCardinality(onClass.decorated()) > 1 ) {
        generateGetterForMany(ofs, onClass);
        ofs << std::endl;
        generateSetterForOne(ofs, onClass);
        ofs << std::endl;
        generateSetterForMany(ofs, onClass);
    }
    if ( _decorated.minCardinality(onClass.decorated()) != _decorated.maxCardinality(onClass.decorated()) ) {
        ofs << std::endl;
        generateRemover(ofs, onClass);
    }
}

void DataPropertyPython::generateKey(std::ostream& ofs, const KlassPython& onClass) const {
    std::string currentClassName = onClass.decorated().prettyIRIName();

    indent(ofs, 1) << R"(""")" << std::endl;
    indent(ofs, 1) << "@brief This method " << genModuleFullyQualified() << "." << currentClassName << ".findBy" << _decorated.prettyIRIName(true) << " returns the only instance of " + onClass.decorated().rdfname() + " with property " + _decorated.rdfname() +  " set to given value." << std::endl;
    indent(ofs, 1) << std::endl;
    indent(ofs, 1) << "@param key value that uniquely identifies the expected object" << std::endl;
    indent(ofs, 1) << std::endl;
    indent(ofs, 1) << "@throw DuplicateObject if more than one object have the same property value" << std::endl;
    indent(ofs, 1) << "@throw ObjectNotFound if no object has given property with value" << std::endl;
    indent(ofs, 1) << R"(""")" << std::endl;

    indent(ofs, 1) << "@staticmethod" << std::endl;
    indent(ofs, 1) << "def findBy" << _decorated.prettyIRIName(true) << "(key):" << std::endl;
    indent(ofs, 2) <<     "return autordf_py.Object.findByKey(\"" << _decorated.rdfname() << "\", key)" << std::endl;
    indent(ofs, 1) << std::endl;
}

void DataPropertyPython::generateGetterForOneMandatory(std::ostream& ofs, const KlassPython& onClass) const {
    std::string methodName = "get" + _decorated.prettyIRIName(true);
    generatePropertyComment(ofs, onClass, methodName, 1,
                    "@return the mandatory value for this property.\n"
                            "@throw PropertyNotFound if value is not set in database");
    indent(ofs, 1) << "def " << methodName << "(self):" << std::endl;
    indent(ofs, 2) << "return self.getPropertyValue(\"" << _decorated.rdfname() << "\")" << std::endl;
    ofs << std::endl;
}

void DataPropertyPython::generateSetterForOne(std::ostream& ofs, const KlassPython& onClass) const {
    std::string methodName = "set" + _decorated.prettyIRIName(true);
    generatePropertyComment(ofs, onClass, methodName, 1,
                    "Sets the mandatory value for this property.\n"
                            "@param value value to set for this property, removing all other values");

    indent(ofs, 1) << "def " << methodName << "(self,value):" << std::endl;
    indent(ofs, 2) << "self.setPropertyValue(\"" << _decorated.rdfname() << "\", value)" << std::endl;
    indent(ofs, 2) << "return self;" << std::endl;
    ofs << std::endl;
}

void DataPropertyPython::generateGetterForOneOptional(std::ostream& ofs, const KlassPython& onClass) const {
    std::string methodName = "get" + _decorated.prettyIRIName(true) + "Optional";
    generatePropertyComment(ofs, onClass, methodName, 1,
                    "@return the value if it is set, or None if it is not set.");

    indent(ofs, 1) << "def " << methodName << "(self):" << std::endl;
    indent(ofs, 2) << "return self.getOptionalPropertyValue(\"" << _decorated.rdfname() << "\")" << std::endl;
    ofs << std::endl;
    methodName = "get" + _decorated.prettyIRIName(true);
    generatePropertyComment(ofs, onClass, methodName, 1,
                    "@return the valueif it is set, or defaultval if it is not set.");

    indent(ofs, 1) << "def " << methodName << "(self, defaultval):" << std::endl;
    indent(ofs, 2) << "ptr = self.getOptionalPropertyValue(\"" << _decorated.rdfname() << "\")" << std::endl;
    indent(ofs, 2) << "return autordf_py.PropertyValue(defaultval) if ptr == None else ptr" << std::endl;
    ofs << std::endl;
}

void DataPropertyPython::generateGetterForMany(std::ostream& ofs, const KlassPython& onClass) const {
    std::string methodName = "get" + _decorated.prettyIRIName(true) + "List";
    generatePropertyComment(ofs, onClass, methodName, 1,
                    "@return the list of values.  List can be empty if not values are set in database");
    indent(ofs, 1) << "def " << methodName << "(self):" << std::endl;
    indent(ofs, 2) << "return self.getPropertyValueList(\"" << _decorated.rdfname() << "\", " << orderedBoolValue() << ")" << std::endl;
    ofs << std::endl;
}

void DataPropertyPython::generateSetterForMany(std::ostream& ofs, const KlassPython& onClass) const {
    std::string methodName = "set" + _decorated.prettyIRIName(true) + "List";
    generatePropertyComment(ofs, onClass, methodName, 1,
                    "Sets property to list of values \n@param values the list of values");
    indent(ofs, 1) << "def " << methodName << "(self, values):" << std::endl;
    indent(ofs, 2) << "self.setPropertyValueList(\"" << _decorated.rdfname() << "\", values, " << orderedBoolValue() << ")" << std::endl;
    indent(ofs, 2) << "return self" << std::endl;
    ofs << std::endl;

    methodName = "add" + _decorated.prettyIRIName(true);
    generatePropertyComment(ofs, onClass, methodName, 1,
                    "Adds a value to a property \n@param value the value to add");

    indent(ofs, 1) << "def " << methodName << "(self, value):" << std::endl;
    indent(ofs, 2) << "self.addPropertyValue(\"" << _decorated.rdfname() << "\", value, " <<
            orderedBoolValue() << ")" << std::endl;
    indent(ofs, 2) << "return self" << std::endl;
    ofs << std::endl;
}

void DataPropertyPython::generateRemover(std::ostream& ofs, const KlassPython& onClass) const {
    std::string methodName = "remove" + _decorated.prettyIRIName(true);
    generatePropertyComment(ofs, onClass, methodName, 1,
                    "Remove a value for this property.\n"
                            "@param value to remove for this property.\n"
                            "@throw PropertyNotFound if propertyIRI has not obj as value\n");
    indent(ofs, 1) << "def " << methodName << "(self, value):" << std::endl;
    indent(ofs, 2) << "self.removePropertyValue(\"" << _decorated.rdfname() << "\", value)" << std::endl;
    indent(ofs, 2) << "return self" << std::endl;
    ofs << std::endl;
}

std::string DataPropertyPython::orderedBoolValue() const {
    return _decorated.ordered() ? "True" : "False";
}

}
}
