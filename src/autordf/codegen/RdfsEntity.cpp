#include "RdfsEntity.h"

#include <stdexcept>
#include <ostream>

#include "Utils.h"

#include <boost/tokenizer.hpp>

namespace autordf {
namespace codegen {

std::string RdfsEntity::outdir = ".";

std::string RdfsEntity::genCppNameSpaceInternal(const char *sep) const {
    std::string prefix = rdfPrefix(_decorated.rdfname, _decorated.model());
    if ( !prefix.empty() ) {
        if ( outdir != "." && sep) {
            return outdir + sep + prefix;
        } else {
            return prefix;
        }
    } else {
        throw std::runtime_error("No prefix found for " + _decorated.rdfname + " RDF resource, unable to use it as C++ namespace");
    }
}

std::string RdfsEntity::genCppNameSpace() const {
    return genCppNameSpaceInternal(nullptr);
}

std::string RdfsEntity::genCppNameSpaceInclusionPath() const {
    return genCppNameSpaceInternal("/");
}

std::string RdfsEntity::genCppNameSpaceFullyQualified() const {
    return genCppNameSpaceInternal("::");
}

std::string RdfsEntity::genCppNameSpaceForGuard() const {
    return genCppNameSpaceInternal("_");
}

std::string RdfsEntity::genCppName(bool uppercaseFirst) const {
    std::string cppname = _decorated.rdfname.substr(_decorated.rdfname.find_last_of("/#:") + 1);
    if ( !::isalpha(cppname[0]) ) {
        cppname = "_" + cppname;
    }
    if ( uppercaseFirst ) {
        cppname[0] = ::toupper(cppname[0]);
    }
    return cppname;
}

std::string RdfsEntity::genCppNameWithNamespace() const {
    return genCppNameSpaceFullyQualified() + "::" + genCppName();
}

void RdfsEntity::generateComment(std::ostream& ofs, unsigned int numIndent, const std::string& additionalComment) const {
    if ( !_decorated.label.empty() || !_decorated.comment.empty() ) {
        indent(ofs, numIndent) << "/**" << std::endl;
        if ( !_decorated.label.empty() ) {
            indent(ofs, numIndent) << " * " << _decorated.label << std::endl;
        }
        if ( !_decorated.comment.empty() ) {
            indent(ofs, numIndent) << " * " << _decorated.comment << std::endl;
        }

        boost::char_separator<char> sep("\n");
        boost::tokenizer<boost::char_separator<char> > tokens(additionalComment, sep);
        for (auto const& token : tokens) {
            indent(ofs, numIndent) << " * " << token << std::endl;
        }

        indent(ofs, numIndent) << " */" << std::endl;
    }
}

}
}

