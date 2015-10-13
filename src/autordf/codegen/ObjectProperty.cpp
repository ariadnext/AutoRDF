#include "ObjectProperty.h"

#include "Klass.h"
#include "Utils.h"

namespace autordf {
namespace codegen {

// Returns class for this property, or nullptr if no class is registered
std::shared_ptr<Klass> ObjectProperty::findClass() const {
    auto kit = Klass::uri2Ptr.find(range);
    if ( kit != Klass::uri2Ptr.end() ) {
        return kit->second;
    }
    return nullptr;
}

void ObjectProperty::generateDeclaration(std::ostream& ofs, const Klass& onClass) const {
    auto propertyClass = findClass();

    ofs << std::endl;

    if ( getEffectiveMaxCardinality(onClass) <= 1 ) {
        if ( getEffectiveMinCardinality(onClass) > 0 ) {
            generateComment(ofs, 1,
                "@return the mandatory instance.\n"
                "@throw PropertyNotFound if object reference is not set\n"
                "@throw DuplicateProperty if database contains more than one value");
            indent(ofs, 1) << propertyClass->genCppNameWithNamespace() << " " << genCppName() << "() const;" << std::endl;
        } else {
            generateComment(ofs, 1,
                            "@return the object instance if it is set, or nullptr if it is not set.\n"
                            "@throw DuplicateProperty if database contains more than one value");
            indent(ofs, 1) << "std::shared_ptr<" << propertyClass->genCppNameWithNamespace()  << "> " << genCppName() << "Optional() const;" << std::endl;
        }
    }
    if ( getEffectiveMaxCardinality(onClass) > 1 ) {
        generateComment(ofs, 1,
                        "@return the list typed objects.  List can be empty if not values are set in database");
        indent(ofs, 1) << "std::list<" << propertyClass->genCppNameWithNamespace()  << "> " << genCppName() << "List() const;" << std::endl;
        ofs << std::endl;
        generateDeclarationSetterForMany(ofs, onClass);
    }
    ofs << std::endl;
    generateDeclarationSetterForOne(ofs, onClass);
    ofs << std::endl;
}

void ObjectProperty::generateDefinition(std::ostream& ofs, const Klass& onClass) const {
    auto propertyClass = findClass();
    std::string currentClassName = "I" + onClass.genCppName();

    if ( getEffectiveMaxCardinality(onClass) <= 1 ) {
        if ( getEffectiveMinCardinality(onClass) > 0 ) {
            ofs << propertyClass->genCppNameWithNamespace() << " " << currentClassName << "::" << genCppName() << "() const {" << std::endl;
            indent(ofs, 1) << "return object().getObject(\"" << rdfname << "\").as<" << propertyClass->genCppNameWithNamespace() << ">();" << std::endl;
            ofs << "}" << std::endl;
        } else {
            ofs << "std::shared_ptr<" << propertyClass->genCppNameWithNamespace() << "> " << currentClassName << "::" << genCppName() << "Optional() const {" << std::endl;
            indent(ofs, 1) << "auto result = object().getOptionalObject(\"" << rdfname << "\");" << std::endl;
            indent(ofs, 1) << "return result ? std::make_shared<" << propertyClass->genCppNameWithNamespace() << ">(*result) : nullptr;" << std::endl;
            ofs << "}" << std::endl;
        }
    }
    if ( getEffectiveMaxCardinality(onClass) > 1 ) {
        ofs << "std::list<" << propertyClass->genCppNameWithNamespace() << "> " << currentClassName << "::" << genCppName() << "List() const {" << std::endl;
        indent(ofs, 1) << "return object().getObjectListImpl<" << propertyClass->genCppNameWithNamespace() << ">(\"" <<  rdfname << "\");" << std::endl;
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
    auto propertyClass = findClass();
    indent(ofs, 1) << "void set" << genCppName(true) << "( const " << propertyClass->genCppNameWithNamespace() << "& value);" << std::endl;
}

void ObjectProperty::generateDeclarationSetterForMany(std::ostream& ofs, const Klass& onClass) const {
    generateComment(ofs, 1,
                    "Sets the values for this property.\n"
                            "@param value value to set for this property, removing all other values");
    auto propertyClass = findClass();
    indent(ofs, 1) << "void set" << genCppName(true) << "( const std::list<" << propertyClass->genCppNameWithNamespace() << ">& values);" << std::endl;
}

void ObjectProperty::generateDefinitionSetterForOne(std::ostream& ofs, const Klass& onClass) const {
    auto propertyClass = findClass();
    std::string currentClassName = "I" + onClass.genCppName();
    ofs << "void " << currentClassName << "::set" << genCppName(true) << "( const " << propertyClass->genCppNameWithNamespace() << "& value) {" << std::endl;
    indent(ofs, 1) <<     "return object().setObject(\"" << rdfname << "\", value);" << std::endl;
    ofs << "}" << std::endl;
}

void ObjectProperty::generateDefinitionSetterForMany(std::ostream& ofs, const Klass& onClass) const {
    auto propertyClass = findClass();
    std::string currentClassName = "I" + onClass.genCppName();
    ofs << "void " << currentClassName << "::set" << genCppName(true) << "( const std::list<" << propertyClass->genCppNameWithNamespace() << ">& values) {" << std::endl;
    indent(ofs, 1) <<     "object().setObjectListImpl<" << propertyClass->genCppNameWithNamespace() << ">(\"" <<  rdfname << "\", values);" << std::endl;
    ofs << "}" << std::endl;
}

std::map<std::string, std::shared_ptr<ObjectProperty> > ObjectProperty::uri2Ptr;

}
}