#include "ObjectProperty.h"

#include "Klass.h"
#include "Utils.h"

namespace autordf {
namespace codegen {

void ObjectProperty::generateDeclaration(std::ostream& ofs, const Klass& onClass) const {
    auto propertyClass = Klass(*_decorated.findClass().get());

    ofs << std::endl;

    if ( _decorated.getEffectiveMaxCardinality(onClass.decorated()) <= 1 ) {
        if ( _decorated.getEffectiveMinCardinality(onClass.decorated()) > 0 ) {
            generateComment(ofs, 1,
                "@return the mandatory instance.\n"
                "@throw PropertyNotFound if object reference is not set\n"
                "@throw DuplicateProperty if database contains more than one value");
            indent(ofs, 1) << propertyClass.genCppNameWithNamespace() << " " << genCppName() << "() const;" << std::endl;
        } else {
            generateComment(ofs, 1,
                            "@return the object instance if it is set, or nullptr if it is not set.\n"
                            "@throw DuplicateProperty if database contains more than one value");
            indent(ofs, 1) << "std::shared_ptr<" << propertyClass.genCppNameWithNamespace()  << "> " << genCppName() << "Optional() const;" << std::endl;
        }
    }
    if ( _decorated.getEffectiveMaxCardinality(onClass.decorated()) > 1 ) {
        generateComment(ofs, 1,
                        "@return the list typed objects.  List can be empty if not values are set in database");
        indent(ofs, 1) << "std::list<" << propertyClass.genCppNameWithNamespace()  << "> " << genCppName() << "List() const;" << std::endl;
        ofs << std::endl;
        generateDeclarationSetterForMany(ofs, onClass);
    }
    ofs << std::endl;
    generateDeclarationSetterForOne(ofs, onClass);
    ofs << std::endl;
}

void ObjectProperty::generateDefinition(std::ostream& ofs, const Klass& onClass) const {
    auto propertyClass = Klass(*_decorated.findClass().get());
    std::string currentClassName = "I" + onClass.genCppName();

    if ( _decorated.getEffectiveMaxCardinality(onClass.decorated()) <= 1 ) {
        if ( _decorated.getEffectiveMinCardinality(onClass.decorated()) > 0 ) {
            ofs << propertyClass.genCppNameWithNamespace() << " " << currentClassName << "::" << genCppName() << "() const {" << std::endl;
            indent(ofs, 1) << "return object().getObject(\"" << _decorated.rdfname << "\").as<" << propertyClass.genCppNameWithNamespace() << ">();" << std::endl;
            ofs << "}" << std::endl;
        } else {
            ofs << "std::shared_ptr<" << propertyClass.genCppNameWithNamespace() << "> " << currentClassName << "::" << genCppName() << "Optional() const {" << std::endl;
            indent(ofs, 1) << "auto result = object().getOptionalObject(\"" << _decorated.rdfname << "\");" << std::endl;
            indent(ofs, 1) << "return result ? std::make_shared<" << propertyClass.genCppNameWithNamespace() << ">(*result) : nullptr;" << std::endl;
            ofs << "}" << std::endl;
        }
    }
    if ( _decorated.getEffectiveMaxCardinality(onClass.decorated()) > 1 ) {
        ofs << "std::list<" << propertyClass.genCppNameWithNamespace() << "> " << currentClassName << "::" << genCppName() << "List() const {" << std::endl;
        indent(ofs, 1) << "return object().getObjectListImpl<" << propertyClass.genCppNameWithNamespace() << ">(\"" <<  _decorated.rdfname << "\");" << std::endl;
        ofs << "}" << std::endl;
        ofs << std::endl;
        generateDefinitionSetterForMany(ofs, onClass);
    }
    ofs << std::endl;
    generateDefinitionSetterForOne(ofs, onClass);
    ofs << std::endl;
}

void ObjectProperty::generateDeclarationSetterForOne(std::ostream& ofs, const Klass& onClass) const {
    generateComment(ofs, 1,
                    "Sets the mandatory value for this property.\n"
                            "@param value value to set for this property, removing all other values");
    auto propertyClass = Klass(*_decorated.findClass().get());
    indent(ofs, 1) << "void set" << genCppName(true) << "( const " << propertyClass.genCppNameWithNamespace() << "& value);" << std::endl;
}

void ObjectProperty::generateDeclarationSetterForMany(std::ostream& ofs, const Klass& onClass) const {
    generateComment(ofs, 1,
                    "Sets the values for this property.\n"
                            "@param value value to set for this property, removing all other values");
    auto propertyClass = Klass(*_decorated.findClass().get());
    indent(ofs, 1) << "void set" << genCppName(true) << "( const std::list<" << propertyClass.genCppNameWithNamespace() << ">& values);" << std::endl;
}

void ObjectProperty::generateDefinitionSetterForOne(std::ostream& ofs, const Klass& onClass) const {
    auto propertyClass = Klass(*_decorated.findClass().get());
    std::string currentClassName = "I" + onClass.genCppName();
    ofs << "void " << currentClassName << "::set" << genCppName(true) << "( const " << propertyClass.genCppNameWithNamespace() << "& value) {" << std::endl;
    indent(ofs, 1) <<     "return object().setObject(\"" << _decorated.rdfname << "\", value);" << std::endl;
    ofs << "}" << std::endl;
}

void ObjectProperty::generateDefinitionSetterForMany(std::ostream& ofs, const Klass& onClass) const {
    auto propertyClass = Klass(*_decorated.findClass().get());
    std::string currentClassName = "I" + onClass.genCppName();
    ofs << "void " << currentClassName << "::set" << genCppName(true) << "( const std::list<" << propertyClass.genCppNameWithNamespace() << ">& values) {" << std::endl;
    indent(ofs, 1) <<     "object().setObjectListImpl<" << propertyClass.genCppNameWithNamespace() << ">(\"" <<  _decorated.rdfname << "\", values);" << std::endl;
    ofs << "}" << std::endl;
}
}
}