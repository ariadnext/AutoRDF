#include <autordf/ontology/Ontology.h>
#include "ObjectProperty.h"

#include "Klass.h"
#include "Utils.h"

namespace autordf {
namespace codegen {

void ObjectProperty::generateDeclaration(std::ostream& ofs, const Klass& onClass) const {
    auto propertyClass = effectiveClass(onClass);

    ofs << std::endl;

    if ( _decorated.maxCardinality(onClass.decorated()) <= 1 ) {
        if ( _decorated.minCardinality(onClass.decorated()) > 0 ) {
            generateComment(ofs, 1,
                "@return the mandatory instance.\n"
                "@throw PropertyNotFound if object reference is not set\n"
                "@throw DuplicateProperty if database contains more than one value", &propertyClass);
            indent(ofs, 1) << propertyClass.genCppNameWithNamespace() << " " << _decorated.prettyIRIName()<< "() const;" << std::endl;
        } else {
            generateComment(ofs, 1,
                            "@return the object instance if it is set, or nullptr if it is not set.\n"
                            "@throw DuplicateProperty if database contains more than one value", &propertyClass);
            indent(ofs, 1) << "std::shared_ptr<" << propertyClass.genCppNameWithNamespace()  << "> " << _decorated.prettyIRIName() << "Optional() const;" << std::endl;
        }
    }
    if ( _decorated.maxCardinality(onClass.decorated()) > 1 ) {
        generateComment(ofs, 1,
                        "@return the list typed objects.  List can be empty if not values are set in database", &propertyClass);
        indent(ofs, 1) << "std::list<" << propertyClass.genCppNameWithNamespace()  << "> " << _decorated.prettyIRIName() << "List() const;" << std::endl;
        ofs << std::endl;
        generateDeclarationSetterForMany(ofs, onClass);
    }
    ofs << std::endl;
    generateDeclarationSetterForOne(ofs, onClass);
    ofs << std::endl;
}

void ObjectProperty::generateDefinition(std::ostream& ofs, const Klass& onClass) const {
    auto propertyClass = effectiveClass(onClass);
    std::string currentClassName = "I" + onClass.decorated().prettyIRIName();

    if ( _decorated.maxCardinality(onClass.decorated()) <= 1 ) {
        if ( _decorated.minCardinality(onClass.decorated()) > 0 ) {
            ofs << propertyClass.genCppNameWithNamespace() << " " << currentClassName << "::" << _decorated.prettyIRIName() << "() const {" << std::endl;
            indent(ofs, 1) << "return object().getObject(\"" << _decorated.rdfname() << "\").as<" << propertyClass.genCppNameWithNamespace() << ">();" << std::endl;
            ofs << "}" << std::endl;
        } else {
            ofs << "std::shared_ptr<" << propertyClass.genCppNameWithNamespace() << "> " << currentClassName << "::" <<
                    _decorated.prettyIRIName() << "Optional() const {" << std::endl;
            indent(ofs, 1) << "auto result = object().getOptionalObject(\"" << _decorated.rdfname() << "\");" << std::endl;
            indent(ofs, 1) << "return result ? std::make_shared<" << propertyClass.genCppNameWithNamespace() << ">(*result) : nullptr;" << std::endl;
            ofs << "}" << std::endl;
        }
    }
    if ( _decorated.maxCardinality(onClass.decorated()) > 1 ) {
        ofs << "std::list<" << propertyClass.genCppNameWithNamespace() << "> " << currentClassName << "::" <<
                _decorated.prettyIRIName() << "List() const {" << std::endl;
        indent(ofs, 1) << "return object().getObjectListImpl<" << propertyClass.genCppNameWithNamespace() << ">(\"" <<  _decorated.rdfname() << "\");" << std::endl;
        ofs << "}" << std::endl;
        ofs << std::endl;
        generateDefinitionSetterForMany(ofs, onClass);
    }
    ofs << std::endl;
    generateDefinitionSetterForOne(ofs, onClass);
    ofs << std::endl;
}

void ObjectProperty::generateDeclarationSetterForOne(std::ostream& ofs, const Klass& onClass) const {
    auto propertyClass = effectiveClass(onClass);
    generateComment(ofs, 1,
                    "Sets the mandatory value for this property.\n"
                            "@param value value to set for this property, removing all other values", &propertyClass);
    indent(ofs, 1) << "void set" << _decorated.prettyIRIName(true) << "( const " << propertyClass.genCppNameWithNamespace() << "& value);" << std::endl;
}

void ObjectProperty::generateDeclarationSetterForMany(std::ostream& ofs, const Klass& onClass) const {
    auto propertyClass = effectiveClass(onClass);
    generateComment(ofs, 1,
                    "Sets the values for this property.\n"
                            "@param values values to set for this property, removing all other values", &propertyClass);
    indent(ofs, 1) << "void set" << _decorated.prettyIRIName(true) << "( const std::list<" << propertyClass.genCppNameWithNamespace() << ">& values);" << std::endl;
    ofs << std::endl;
    generateComment(ofs, 1,
                    "Adds a value for this property.\n"
                            "@param value value to set for this property, removing all other values", &propertyClass);
    indent(ofs, 1) << "void add" << _decorated.prettyIRIName(true) << "( const " << propertyClass.genCppNameWithNamespace() << "& value);" << std::endl;
}

void ObjectProperty::generateDefinitionSetterForOne(std::ostream& ofs, const Klass& onClass) const {
    auto propertyClass = effectiveClass(onClass);
    std::string currentClassName = "I" + onClass.decorated().prettyIRIName();
    ofs << "void " << currentClassName << "::set" << _decorated.prettyIRIName(true) << "( const " << propertyClass.genCppNameWithNamespace() << "& value) {" << std::endl;
    indent(ofs, 1) <<     "return object().setObject(\"" << _decorated.rdfname() << "\", value);" << std::endl;
    ofs << "}" << std::endl;
}

void ObjectProperty::generateDefinitionSetterForMany(std::ostream& ofs, const Klass& onClass) const {
    auto propertyClass = effectiveClass(onClass);
    std::string currentClassName = "I" + onClass.decorated().prettyIRIName();
    ofs << "void " << currentClassName << "::set" << _decorated.prettyIRIName(true) << "( const std::list<" << propertyClass.genCppNameWithNamespace() << ">& values) {" << std::endl;
    indent(ofs, 1) <<     "object().setObjectListImpl<" << propertyClass.genCppNameWithNamespace() << ">(\"" <<  _decorated.rdfname() << "\", values);" << std::endl;
    ofs << "}" << std::endl;
    ofs << std::endl;
    ofs << "void " << currentClassName << "::add" << _decorated.prettyIRIName(true) << "( const " << propertyClass.genCppNameWithNamespace() << "& value) {" << std::endl;
    indent(ofs, 1) <<     "return object().addObject(\"" << _decorated.rdfname() << "\", value);" << std::endl;
    ofs << "}" << std::endl;
}

Klass ObjectProperty::effectiveClass(const Klass& onClass) const {
    std::shared_ptr<ontology::Klass> kls = _decorated.findClass(&onClass.decorated());
    if ( kls ) {
        return *kls;
    } else {
        return _decorated.ontology()->findClass(autordf::ontology::Ontology::OWL_NS + "Thing");
    }
}


}
}