#ifndef AUTORDF_CODEGEN_RDFSENDITY_H
#define AUTORDF_CODEGEN_RDFSENDITY_H

#include <string>
#include <iosfwd>
#include <autordf/ontology/RdfsEntity.h>

namespace autordf {

class Model;

namespace codegen {

class KlassPython;

class RdfsEntityPython {
    ontology::RdfsEntity _decorated;
public:
    RdfsEntityPython(const ontology::RdfsEntity& decorated) : _decorated(decorated) {}

    const ontology::RdfsEntity& decorated() const { return _decorated; }

    // Where code generation will happen
    static std::string outdir;

    // Lang preference in order for documentation generation
    static std::vector<std::string> preferredLang;

    // Check if used
    // [outfolder.]namespace.[I]classname
    std::string genNameWithModule(bool interfaceMode = false) const;

    // Check if used
    // [outfolder.]namespace
    std::string genModuleFullyQualified() const;

    // module
    std::string genModule() const;

    // [outfolder/]namespace
    std::string genModuleInclusionPath() const;

    void generateComment(std::ostream& ofs, unsigned int numIndent, const std::string& additionalComment = "", const RdfsEntityPython *alternate = 0) const;

    void generatePropertyComment(std::ostream& ofs, const KlassPython& onClass, const std::string& methodName, unsigned int numIndent, const std::string& additionalComment = "", const RdfsEntityPython *alternate = 0) const;
private:
    std::string genModuleInternal(const char *sep) const;
};

}
}

#endif //AUTORDF_CODEGEN_RDFSENDITY_H
