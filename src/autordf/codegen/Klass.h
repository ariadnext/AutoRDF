#pragma once

#include <memory>

#include <inja/inja.hpp>

#include "autordf/ontology/Klass.h"

namespace autordf {
namespace codegen {
/**
 * This class wrap around the ontology Klass to add the functions dedicated to code generation.
 * Inherit to generate classes in another language
 */
class Klass {
public:
    /**
     * Constructs a class for rendering
     *
     * @param ontology the class ontology
     * @param renderer the renderer to use to render this class
     */
    Klass(const ontology::Klass& ontology, inja::Environment& renderer);

    /**
     * Constructs a class for use as a dependency
     *
     * @param ontology the class ontology
     */
    explicit Klass(const ontology::Klass& ontology);

    virtual ~Klass() = default;

    /**
     * Builds the Json representation of the class ontology
     */
    void buildTemplateData(bool withDependency = true);

    /**
     * Gives the Json representation of the class ontology
     *
     * @return the Json representation of the class ontology
     * @see https://confluence.rennes.ariadnext.com/display/OCR/AutoRDF+template+system
     */
    nlohmann::json templateData() const {
        return _templateData;
    }

    /**
     * Implement this function for your target language
     */
    virtual void generate() const = 0;

    std::string className() const;
    std::string interfaceName() const;
    std::string packagePath() const;
    std::string basePackageName() const;
    std::string fullPackageName() const;

protected:
    mutable inja::Environment _renderer;
    std::string _interfacePrefix = "I";
    std::string _packageSeparator = "::";
    std::string _pathSeparator = "/";

private:
    const ontology::Klass& _ontology;
    nlohmann::json _templateData;

    virtual std::unique_ptr<Klass> buildDependency(const ontology::Klass& ontology) const = 0;
    void buildDependencies();
    void buildAnnotations();
    void buildDataProperties();
    void buildObjectProperties();
    void buildOneOfValues();
    void buildAncestors();

    nlohmann::json buildProperty(const ontology::Property& property) const;
    nlohmann::json buildPropertyKlass(const ontology::Property& property) const;
    nlohmann::json buildCommentSection(const ontology::RdfsEntity& input) const;
    nlohmann::json buildComment(const std::vector<PropertyValue>& input) const;

    nlohmann::json getPrefix() const;
    nlohmann::json getPackages() const;
};
}
}
