#include "Klass.h"

#include <inja/inja.hpp>

#include "autordf/ontology/Ontology.h"
#include "autordf/ontology/Klass.h"
#include "autordf/Model.h"

#include "Environment.h"

namespace autordf {
namespace codegen {
Klass::Klass(const ontology::Klass& ontology, inja::Environment& renderer) :
    _renderer(renderer),
    _ontology(ontology),
    _templateData() {
}

Klass::Klass(const ontology::Klass& ontology) :
    _renderer(),
    _ontology(ontology),
    _templateData() {

}

void Klass::buildTemplateData(bool withDependency) {
    nlohmann::json packages;
    packages["packages"] = getPackages();

    _templateData["className"] = _ontology.prettyIRIName();
    _templateData["interfaceName"] = _interfacePrefix + _ontology.prettyIRIName();
    _templateData["rdfName"] = _ontology.rdfname().c_str();
    _templateData["packages"] = getPackages();
    _templateData["packagePath"] = inja::render("{{ join(packages, \"/\") }}", packages);
    _templateData["basePackageName"] = inja::render("{{ last(packages) }}", packages);
    _templateData["fullPackageName"] = inja::render("{{ join(packages, \"" + _packageSeparator + "\") }}", packages);
    _templateData["fullPackagePath"] = inja::render("{{ join(packages, \"" + _pathSeparator + "\") }}", packages);
    _templateData["fullClassName"] = _templateData.value("fullPackageName", "") + _packageSeparator + _templateData.value("className", "");
    _templateData["fullInterfaceName"] = _templateData.value("fullPackageName", "") + _packageSeparator + _templateData.value("interfaceName", "");
    _templateData["hasOneOf"] = !_ontology.oneOfValues().empty();
    _templateData["classComment"] = buildCommentSection(_ontology);

    if (withDependency) {
        buildDependencies();
        buildAnnotations();
        buildDataProperties();
        buildObjectProperties();
        buildOneOfValues();
        buildAncestors();
    }
}

std::string Klass::className() const {
    return _templateData["className"];
}

std::string Klass::interfaceName() const {
    return _templateData["interfaceName"];
}

std::string Klass::packagePath() const {
    return _templateData["packagePath"];
}

std::string Klass::basePackageName() const {
    return _templateData["basePackageName"];
}

std::string Klass::fullPackageName() const {
    return _templateData["fullPackageName"];
}

void Klass::buildDependencies() {
    std::set<ontology::Klass> deps;
    std::set<std::shared_ptr<const ontology::ObjectProperty> > objects;

    std::copy(_ontology.objectProperties().begin(), _ontology.objectProperties().end(), std::inserter(objects, objects.begin()));
    std::copy(_ontology.objectKeys().begin(), _ontology.objectKeys().end(), std::inserter(objects, objects.begin()));

    for ( const std::shared_ptr<const ontology::ObjectProperty>& p : objects) {
        auto val = p->findClass(&_ontology);
        if ( val ) {
            if ( val->prettyIRIName() != _ontology.prettyIRIName() ) {
                deps.emplace(*val);
            }
        } else {
            deps.emplace(*(_ontology.ontology()->findClass(autordf::ontology::Ontology::OWL_NS + "Thing")));
        }
    }

    _templateData["dependencies"] = nlohmann::json::array();
    for (auto const& dep : deps) {
            auto klass = buildDependency(dep);
            klass->buildTemplateData(false);
            _templateData["dependencies"].push_back(klass->templateData());
    }
}

void Klass::buildAnnotations() {
    _templateData["annotationProperties"] = nlohmann::json::array();
    _templateData["annotationKeys"] = nlohmann::json::array();

    for (auto const& property : _ontology.annotationProperties()) {
        _templateData["annotationProperties"].push_back(buildProperty(*property));
    }

    for (auto const& property : _ontology.annotationKeys()) {
        _templateData["annotationKeys"].push_back(buildProperty(*property));
    }
}

void Klass::buildDataProperties() {
    _templateData["dataProperties"] = nlohmann::json::array();
    _templateData["dataKeys"] = nlohmann::json::array();

    for (auto const& property : _ontology.dataProperties()) {
        _templateData["dataProperties"].push_back(buildProperty(*property));
    }

    for (auto const& property : _ontology.dataKeys()) {
        _templateData["dataKeys"].push_back(buildProperty(*property));
    }
}

void Klass::buildObjectProperties() {
    _templateData["objectProperties"] = nlohmann::json::array();
    _templateData["objectKeys"] = nlohmann::json::array();

    for (auto const& property : _ontology.objectProperties()) {
        _templateData["objectProperties"].push_back(buildProperty(*property));
    }

    for (auto const& property : _ontology.objectKeys()) {
        _templateData["objectKeys"].push_back(buildProperty(*property));
    }

}

void Klass::buildOneOfValues() {
    _templateData["oneOfValues"] = nlohmann::json::array();

    for (auto const& oneOfValue : _ontology.oneOfValues()) {
        nlohmann::json jsonValue;

        jsonValue["name"] = oneOfValue.prettyIRIName();
        jsonValue["rdfName"] = oneOfValue.rdfname().c_str();
        jsonValue["comment"] = buildCommentSection(oneOfValue);

        _templateData["oneOfValues"].push_back(jsonValue);
    }
}

void Klass::buildAncestors() {
    _templateData["ancestors"] = nlohmann::json::array();

    for (auto const& ancestor : _ontology.getAllAncestors()) {
        auto ancestorKlass = buildDependency(*ancestor);
        ancestorKlass->buildTemplateData(false);
        _templateData["ancestors"].push_back(ancestorKlass->templateData());
    }
}

nlohmann::json Klass::buildProperty(const ontology::Property& property) const {
    nlohmann::json jsonProperty;

    jsonProperty["name"] = property.prettyIRIName();
    jsonProperty["fullName"] = property.rdfname().prettyName();
    jsonProperty["rdfName"] = property.rdfname().c_str();
    jsonProperty["minCardinality"] = property.minCardinality(_ontology);
    jsonProperty["maxCardinality"] = property.maxCardinality(_ontology);
    jsonProperty["ordered"] = property.ordered();
    jsonProperty["comment"] = buildCommentSection(property);
    jsonProperty["class"] = buildPropertyKlass(property);

    auto foundType = cvt::rdfMapType.find(property.range(&_ontology));
    if (foundType != cvt::rdfMapType.end()) {
        jsonProperty["type"]["undefined"] = false;
        jsonProperty["type"]["value"] = foundType->second;
        jsonProperty["type"]["rdf"] = cvt::rdfTypeEnumString(foundType->second);
    } else {
        jsonProperty["type"]["undefined"] = true;
        jsonProperty["type"]["value"] = -1;
        jsonProperty["type"]["rdf"] = "";
    }

    return jsonProperty;
}

nlohmann::json Klass::buildPropertyKlass(const ontology::Property& property) const {
    try {
        // We can only generate property class for an ObjectProperty
        auto objectProperty = reinterpret_cast<const ontology::ObjectProperty&>(property);

        auto propertyOntology = objectProperty.findClass(&_ontology);
        if (!propertyOntology) {
            propertyOntology = std::make_shared<ontology::Klass>(*objectProperty.ontology()->findClass(autordf::ontology::Ontology::OWL_NS + "Thing"));
        }

        auto propertyKlass = buildDependency(*propertyOntology);
        propertyKlass->buildTemplateData(false);
        auto json = propertyKlass->templateData();
        return json;
    } catch (const std::bad_cast& e) {
        return {};
    }
}

nlohmann::json Klass::buildCommentSection(const ontology::RdfsEntity& input) const {
    nlohmann::json section;

    section["isDoxygen"] = !input.label().empty() || !input.comment().empty();
    if (!input.label().empty()) {
        section["label"] = Environment::propertyI18(input.label());
    }
    if (!input.comment().empty()) {
        section["comment"] = buildComment(input.comment());
    }
    if (!input.seeAlso().empty()) {
        section["seeAlso"] = Environment::propertyI18(input.seeAlso());
    }
    if (!input.isDefinedBy().empty()) {
        section["isDefinedBy"] = Environment::propertyI18(input.isDefinedBy());
    }

    return section;
}

nlohmann::json Klass::buildComment(const std::vector<PropertyValue>& input) const {
    auto comment = nlohmann::json::array();

    const std::string& translation = Environment::propertyI18(input);
    boost::tokenizer<boost::char_separator<char> > lines(translation, Environment::RDF_COMMENT_NEWLINE);
    for (const std::string& line : lines) {
        if ( !line.empty() ) {
            comment.push_back(line);
        }
    }

    return comment;
}

nlohmann::json Klass::getPrefix() const {
    std::string prefix = _ontology.ontology()->model()->iriPrefix(_ontology.rdfname());

    if (!prefix.empty()) {
        return {prefix};
    } else {
        throw std::runtime_error("No prefix found for " + _ontology.rdfname() + " RDF resource");
    }
}

nlohmann::json Klass::getPackages() const {
    std::string prefix = _ontology.ontology()->model()->iriPrefix(_ontology.rdfname());

    if (!prefix.empty()) {
        if (!Environment::namespace_.empty()) {
            return {Environment::namespace_, prefix};
        } else {
            return {prefix};
        }
    } else {
        throw std::runtime_error("No prefix found for " + _ontology.rdfname() + " RDF resource");
    }
}
}
}
