#ifndef AUTORDF_CODEGEN_RDFSENDITY_H
#define AUTORDF_CODEGEN_RDFSENDITY_H

#include <string>
#include <iosfwd>

namespace autordf {

class Model;

namespace codegen {

class RdfsEntity {
public:
    // Object iri
    std::string rdfname;

    // rdfs comment
    std::string comment;
    // rdfs label
    std::string label;

    std::string genCppName() const;

    std::string genCppNameSpace() const;

    std::string genCppNameWithNamespace() const;

    void generateComment(std::ostream& ofs, unsigned int numIndent, const std::string& additionalComment = "") const;

    static void setModel(Model *m) {
        _m = m;
    }

    static const Model* model() { return _m; }

private:
    static Model *_m;
};

}
}

#endif //AUTORDF_CODEGEN_RDFSENDITY_H
