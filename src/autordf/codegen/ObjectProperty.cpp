#include <autordf/ontology/Ontology.h>
#include "ObjectProperty.h"

#include "Klass.h"
#include "Utils.h"

namespace autordf {
namespace codegen {

void ObjectProperty::generateDeclaration(std::ostream& ofs, const Klass& onClass) const {
    auto propertyClass = effectiveClass(onClass);

    ofs << std::endl;
    indent(ofs, 1) << "/**" << std::endl;
    indent(ofs, 1) << " * Full iri for " <<  _decorated.rdfname().prettyName(false) << " object property." << std::endl;
    indent(ofs, 1) << " */" << std::endl;
    indent(ofs, 1) << "static const autordf::Uri " << _decorated.prettyIRIName() << "ObjectPropertyIri;" << std::endl;
    ofs << std::endl;

    if ( _decorated.maxCardinality(onClass.decorated()) <= 1 ) {
        if ( _decorated.minCardinality(onClass.decorated()) > 0 ) {
            generateComment(ofs, 1,
                "@return the mandatory instance.\n"
                "@throw PropertyNotFound if object reference is not set", &propertyClass);
            indent(ofs, 1) << propertyClass.genCppNameWithNamespace() << " " << _decorated.prettyIRIName()<< "() const;" << std::endl;
        } else {
            generateComment(ofs, 1,
                            "@return the object instance if it is set, or nullptr if it is not set.", &propertyClass);
            indent(ofs, 1) << "std::shared_ptr<" << propertyClass.genCppNameWithNamespace()  << "> " << _decorated.prettyIRIName() << "Optional() const;" << std::endl;
        }
    }
    if ( _decorated.maxCardinality(onClass.decorated()) > 1 ) {
        generateComment(ofs, 1,
                        "@return the list typed objects.  List can be empty if not values are set in database", &propertyClass);
        indent(ofs, 1) << "std::vector<" << propertyClass.genCppNameWithNamespace()  << "> " << _decorated.prettyIRIName() << "List() const;" << std::endl;
        ofs << std::endl;
        generateDeclarationSetterForMany(ofs, onClass);
    }
    ofs << std::endl;
    generateDeclarationSetterForOne(ofs, onClass);
    if ( _decorated.minCardinality(onClass.decorated()) != _decorated.maxCardinality(onClass.decorated()) ) {
        ofs << std::endl;
        generateRemoverDeclaration(ofs, onClass);
    }
    ofs << std::endl;
}

void ObjectProperty::generateKeyDeclaration(std::ostream& ofs, const Klass& onClass) const {
    auto propertyClass = effectiveClass(onClass);
    std::string currentClassName = onClass.decorated().prettyIRIName();

    indent(ofs, 1) << "/**" << std::endl;
    indent(ofs, 1) << " * @brief Returns the only instance of " + currentClassName + " with property " + _decorated.prettyIRIName() +  " set to given object." << std::endl;
    indent(ofs, 1) << " * " << std::endl;
    indent(ofs, 1) << " * @param key value that uniquely identifies the expected object" << std::endl;
    indent(ofs, 1) << " * " << std::endl;
    indent(ofs, 1) << " * @throw DuplicateObject if more than one object have the same property value" << std::endl;
    indent(ofs, 1) << " * @throw ObjectNotFound if no object has given property with value" << std::endl;
    indent(ofs, 1) << " */" << std::endl;

    indent(ofs, 1) << "static " << currentClassName << " findBy" << _decorated.prettyIRIName(true) << "( const " << propertyClass.genCppNameWithNamespace() << "& key ) {" << std::endl;
    indent(ofs, 2) <<     "return findByKey(\"" << _decorated.rdfname() << "\", reinterpret_cast<const ::autordf::Object&>(key)).as<" << currentClassName << ">();" << std::endl;
    indent(ofs, 1) << "}" << std::endl;
    indent(ofs, 1) << std::endl;
}

void ObjectProperty::generateDefinition(std::ostream& ofs, const Klass& onClass) const {
    auto propertyClass = effectiveClass(onClass);
    std::string currentClassName = "I" + onClass.decorated().prettyIRIName();

    ofs << "const autordf::Uri " << currentClassName << "::" << _decorated.prettyIRIName() << "ObjectPropertyIri = \"" << _decorated.rdfname() << "\";" << std::endl;
    ofs << std::endl;

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
        ofs << "std::vector<" << propertyClass.genCppNameWithNamespace() << "> " << currentClassName << "::" <<
                _decorated.prettyIRIName() << "List() const {" << std::endl;
        indent(ofs, 1) << "return object().getObjectListImpl<" << propertyClass.genCppNameWithNamespace() << ">(\"" <<  _decorated.rdfname() << "\", false);" << std::endl;
        ofs << "}" << std::endl;
        ofs << std::endl;
        generateDefinitionSetterForMany(ofs, onClass);
    }
    ofs << std::endl;
    generateDefinitionSetterForOne(ofs, onClass);
    if ( _decorated.minCardinality(onClass.decorated()) != _decorated.maxCardinality(onClass.decorated()) ) {
        ofs << std::endl;
        generateRemoverDefinition(ofs, onClass);
    }
    ofs << std::endl;
}

void ObjectProperty::generateDeclarationSetterForOne(std::ostream& ofs, const Klass& onClass) const {
    auto propertyClass = effectiveClass(onClass);

    std::string currentClassName = "I" + onClass.decorated().prettyIRIName();

    generateComment(ofs, 1,
                    "Sets the mandatory value for this property.\n"
                            "@param value value to set for this property, removing all other values", &propertyClass);
    indent(ofs, 1) << currentClassName << "& set" << _decorated.prettyIRIName(true) << "( const " << propertyClass.genCppNameWithNamespace(true) << "& value);" << std::endl;
}

void ObjectProperty::generateDeclarationSetterForMany(std::ostream& ofs, const Klass& onClass) const {
    auto propertyClass = effectiveClass(onClass);

    std::string currentClassName = "I" + onClass.decorated().prettyIRIName();

    generateComment(ofs, 1,
                    "Sets the values for this property.\n"
                            "@param values values to set for this property, removing all other values", &propertyClass);
    indent(ofs, 1) << currentClassName << "& set" << _decorated.prettyIRIName(true) << "List( const std::vector<" << propertyClass.genCppNameWithNamespace(false) << ">& values);" << std::endl;
    ofs << std::endl;
    generateComment(ofs, 1,
                    "Adds a value for this property.\n"
                            "@param value value to set for this property, removing all other values", &propertyClass);
    indent(ofs, 1) << currentClassName << "& add" << _decorated.prettyIRIName(true) << "( const " << propertyClass.genCppNameWithNamespace(true) << "& value);" << std::endl;
}

void ObjectProperty::generateDefinitionSetterForOne(std::ostream& ofs, const Klass& onClass) const {
    auto propertyClass = effectiveClass(onClass);
    std::string currentClassName = "I" + onClass.decorated().prettyIRIName();
    ofs << currentClassName << "& " << currentClassName << "::set" << _decorated.prettyIRIName(true) << "( const " << propertyClass.genCppNameWithNamespace(true) << "& value) {" << std::endl;
    indent(ofs, 1) <<     "object().setObject(\"" << _decorated.rdfname() << "\", value.object());" << std::endl;
    indent(ofs, 1) <<     "return *this;" << std::endl;
    ofs << "}" << std::endl;
}

void ObjectProperty::generateDefinitionSetterForMany(std::ostream& ofs, const Klass& onClass) const {
    auto propertyClass = effectiveClass(onClass);
    std::string currentClassName = "I" + onClass.decorated().prettyIRIName();
    ofs << currentClassName << "& " << currentClassName << "::set" << _decorated.prettyIRIName(true) << "List( const std::vector<" << propertyClass.genCppNameWithNamespace(false) << ">& values) {" << std::endl;
    indent(ofs, 1) <<     "object().setObjectListImpl<" << propertyClass.genCppNameWithNamespace(false) << ">(\"" <<  _decorated.rdfname() << "\", values, false);" << std::endl;
    indent(ofs, 1) <<     "return *this;" << std::endl;
    ofs << "}" << std::endl;
    ofs << std::endl;
    ofs << currentClassName << "& " << currentClassName << "::add" << _decorated.prettyIRIName(true) << "( const " << propertyClass.genCppNameWithNamespace(true) << "& value) {" << std::endl;
    indent(ofs, 1) <<     "object().addObject(\"" << _decorated.rdfname() << "\", value.object());" << std::endl;
    indent(ofs, 1) <<     "return *this;" << std::endl;
    ofs << "}" << std::endl;
}

Klass ObjectProperty::effectiveClass(const Klass& onClass) const {
    std::shared_ptr<ontology::Klass> kls = _decorated.findClass(&onClass.decorated());
    if ( kls ) {
        return *kls;
    } else {
        return *_decorated.ontology()->findClass(autordf::ontology::Ontology::OWL_NS + "Thing");
    }
}

void ObjectProperty::generateRemoverDeclaration(std::ostream& ofs, const Klass& onClass) const {
    auto propertyClass = effectiveClass(onClass);

    std::string currentClassName = "I" + onClass.decorated().prettyIRIName();

    generateComment(ofs, 1,
                    "Remove a value for this property.\n"
                            "@param obj value to remove for this property.\n"
                            "@throw PropertyNotFound if propertyIRI has not obj as value", &propertyClass);
    indent(ofs, 1) << currentClassName << "& remove" << _decorated.prettyIRIName(true) << "( const " << propertyClass.genCppNameWithNamespace(true) << "& obj);" << std::endl;
}

void ObjectProperty::generateRemoverDefinition(std::ostream& ofs, const Klass& onClass) const {
    auto propertyClass = effectiveClass(onClass);

    std::string currentClassName = "I" + onClass.decorated().prettyIRIName();

    ofs << currentClassName << "& " << currentClassName << "::remove" << _decorated.prettyIRIName(true) << "( const " << propertyClass.genCppNameWithNamespace(true) << "& value) {" << std::endl;
    indent(ofs, 1) << "object().removeObject(\"" << _decorated.rdfname() << "\", value.object());" << std::endl;
    indent(ofs, 1) << "return *this;" << std::endl;
    ofs  << "}" << std::endl;
}

}
}