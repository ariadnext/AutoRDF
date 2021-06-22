#ifndef AUTORDF_CODEGEN_RDFSENDITY_H
#define AUTORDF_CODEGEN_RDFSENDITY_H

#include <string>
#include <iosfwd>
#include <autordf/ontology/RdfsEntity.h>

namespace autordf {

class Model;

namespace codegen {

class Klass;

class RdfsEntity {
    ontology::RdfsEntity _decorated;
public:
    RdfsEntity(const ontology::RdfsEntity& decorated) : _decorated(decorated) {}

    const ontology::RdfsEntity& decorated() const { return _decorated; }

    // Where code generation will happen
    static std::string outdir;

    // Lang preference in order for documentation generation
    static std::vector<std::string> preferredLang;

    // [outfolder::]namespace::[I]classname
    std::string genCppNameWithNamespace(bool interfaceMode = false) const;

    // [outfolder::]namespace
    std::string genCppNameSpaceFullyQualified() const;

    // namespace
    std::string genCppNameSpace() const;

    // [outfolder/]namespace
    std::string genCppNameSpaceInclusionPath() const;

    // [OURFOLDER_]NAMESPACE
    std::string genCppNameSpaceForGuard() const;

    void generateComment(std::ostream& ofs, unsigned int numIndent, const std::string& additionalComment = "", const RdfsEntity *alternate = 0) const;

    void generatePropertyComment(std::ostream& ofs, const Klass& onClass, const std::string& methodName, unsigned int numIndent, const std::string& additionalComment = "", const RdfsEntity *alternate = 0) const;
private:
    std::string genCppNameSpaceInternal(const char *sep) const;
};

}
}

#endif //AUTORDF_CODEGEN_RDFSENDITY_H
