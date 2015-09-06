#include "RdfsEndity.h"

#include <stdexcept>
#include <ostream>

#include "Utils.h"

#include <boost/tokenizer.hpp>

namespace autordf {
namespace codegen {

std::string RdfsEntity::genCppNameSpace() const {
    std::string prefix = rdfPrefix(rdfname, _m);
    if ( !prefix.empty() ) {
        return prefix;
    } else {
        throw std::runtime_error("No prefix found for " + rdfname + " RDF resource, unable to use it as C++ namespace");
    }
}

std::string RdfsEntity::genCppName() const {
    return codegen::genCppName(rdfname);
}

std::string RdfsEntity::genCppNameWithNamespace() const {
    return genCppNameSpace() + "::" + genCppName();
}

void RdfsEntity::generateComment(std::ostream& ofs, unsigned int numIndent, const std::string& additionalComment) const {
    if ( !label.empty() || !comment.empty() ) {
        indent(ofs, numIndent) << "/**" << std::endl;
        if ( !label.empty() ) {
            indent(ofs, numIndent) << " * " << label << std::endl;
        }
        if ( !comment.empty() ) {
            indent(ofs, numIndent) << " * " << comment << std::endl;
        }

        boost::char_separator<char> sep("\n");
        boost::tokenizer<boost::char_separator<char> > tokens(additionalComment, sep);
        for (auto const& token : tokens) {
            indent(ofs, numIndent) << " * " << token << std::endl;
        }

        indent(ofs, numIndent) << " */" << std::endl;
    }
}

Model *RdfsEntity::_m = 0;

}
}

