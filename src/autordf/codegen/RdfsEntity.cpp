#include "RdfsEntity.h"

#include <stdexcept>
#include <ostream>

#include "Utils.h"

#include <boost/tokenizer.hpp>

namespace autordf {
namespace codegen {

std::string RdfsEntity::outdir = ".";

std::string RdfsEntity::genCppNameSpaceInternal(const char *sep) const {
    std::string prefix = rdfPrefix(_decorated.rdfname(), _decorated.model());
    if ( !prefix.empty() ) {
        if ( outdir != "." && sep) {
            return outdir + sep + prefix;
        } else {
            return prefix;
        }
    } else {
        throw std::runtime_error("No prefix found for " + _decorated.rdfname() + " RDF resource, unable to use it as C++ namespace");
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

std::string RdfsEntity::genCppNameWithNamespace() const {
    return genCppNameSpaceFullyQualified() + "::" + _decorated.prettyIRIName();
}

void RdfsEntity::generateComment(std::ostream& ofs, unsigned int numIndent, const std::string& additionalComment, const RdfsEntity *alternate) const {
    const RdfsEntity *used = this;
    if ( used->_decorated.label().empty() && used->_decorated.comment().empty() ) {
        if ( alternate ) {
            used = alternate;
        }
    }
    if ( !used->_decorated.label().empty() || !used->_decorated.comment().empty() ) {
        indent(ofs, numIndent) << "/**" << std::endl;
        if ( !used->_decorated.label().empty() ) {
            indent(ofs, numIndent) << " * @brief " << used->_decorated.label() << std::endl;
            indent(ofs, numIndent) << " * " << std::endl;
        }
        if ( !used->_decorated.comment().empty() ) {
            indent(ofs, numIndent) << " * " << used->_decorated.comment() << std::endl;
        }
        if ( !used->_decorated.seeAlso().empty() ) {
            indent(ofs, numIndent) << " * @see " << used->_decorated.seeAlso() << std::endl;
        }
        if ( !used->_decorated.isDefinedBy().empty() ) {
            indent(ofs, numIndent) << " * @see " << used->_decorated.isDefinedBy() << std::endl;
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

