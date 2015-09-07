#ifndef AUTORDF_CODEGEN_RDFSENDITY_H
#define AUTORDF_CODEGEN_RDFSENDITY_H

#include <string>
#include <iosfwd>

namespace autordf {

class Model;

namespace codegen {

class RdfsEntity {
public:
    // Where code generation will happen
    static std::string outdir;

    // Object iri
    std::string rdfname;

    // rdfs comment
    std::string comment;
    // rdfs label
    std::string label;

    // classname
    std::string genCppName(bool upperCaseFirst = false) const;

    // [outfolder::]namespace::classname
    std::string genCppNameWithNamespace() const;

    // [outfolder::]namespace
    std::string genCppNameSpaceFullyQualified() const;

    // namespace
    std::string genCppNameSpace() const;

    // [outfolder/]namespace
    std::string genCppNameSpaceInclusionPath() const;

    // [OURFOLDER_]NAMESPACE
    std::string genCppNameSpaceForGuard() const;

    void generateComment(std::ostream& ofs, unsigned int numIndent, const std::string& additionalComment = "") const;

    static void setModel(Model *m) {
        _m = m;
    }

    static const Model* model() { return _m; }

    bool operator<(const RdfsEntity& other) const { return rdfname < other.rdfname; }

private:
    std::string genCppNameSpaceInternal(const char *sep) const;

    static Model *_m;
};

}
}

#endif //AUTORDF_CODEGEN_RDFSENDITY_H
