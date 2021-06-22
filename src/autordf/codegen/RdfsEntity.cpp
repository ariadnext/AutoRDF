#include "RdfsEntity.h"

#include <ostream>
#include <stdexcept>

#include <autordf/I18StringVector.h>
#include <autordf/ontology/Ontology.h>
#include <autordf/Model.h>
#include <autordf/codegen/Klass.h>
#include "Utils.h"

#include <boost/tokenizer.hpp>

namespace autordf {
namespace codegen {

std::string RdfsEntity::outdir = ".";
std::vector<std::string> RdfsEntity::preferredLang = {"en", "fr"};

std::string RdfsEntity::genCppNameSpaceInternal(const char *sep) const {
    std::string prefix = _decorated.ontology()->model()->iriPrefix(_decorated.rdfname());
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

std::string RdfsEntity::genCppNameWithNamespace(bool interfaceMode) const {
    return genCppNameSpaceFullyQualified() + "::" + (interfaceMode ? "I" : "") + _decorated.prettyIRIName();
}

std::string langPreferenceString(const std::vector<PropertyValue>& values, const std::vector<std::string>& preferredLang) {
    if (values.empty()) {
        return "";
    }
    try {
        autordf::I18StringVector langString(values);
        return langString.langPreferenceString(preferredLang);
    } catch (DataConvertionFailure&) {
        return values.front();
    }
}

void RdfsEntity::generateComment(std::ostream& ofs, unsigned int numIndent, const std::string& additionalComment, const RdfsEntity *alternate) const {
    static const boost::char_separator<char> NEWLINE("\r\n");
    const RdfsEntity *used = this;
    if ( used->_decorated.label().empty() && used->_decorated.comment().empty() ) {
        if ( alternate ) {
            used = alternate;
        }
    }
    bool outputDoxygen = !used->_decorated.label().empty() || !used->_decorated.comment().empty();
    if (outputDoxygen) {
        indent(ofs, numIndent) << "/**" << std::endl;
    } else {
        indent(ofs, numIndent) << "/*" << std::endl;
    }
    if ( !used->_decorated.label().empty() ) {
        indent(ofs, numIndent) << " * @brief " << langPreferenceString(used->_decorated.label(), preferredLang) << std::endl;
        indent(ofs, numIndent) << " * " << std::endl;
    }
    if ( !used->_decorated.comment().empty() ) {
        // windows lib needs a local reference for iterator to work
        // was getting "cannot dereference string iterator because the iterator ..."
        const std::string& comment = langPreferenceString(used->_decorated.comment(), preferredLang);
        boost::tokenizer<boost::char_separator<char> > lines(comment, NEWLINE);
        for (const std::string& line : lines) {
            if ( !line.empty() ) {
                indent(ofs, numIndent) << " * " << line << std::endl;
            }
        }
    }

    if ( !used->_decorated.seeAlso().empty() ) {
        indent(ofs, numIndent) << " * @see " << langPreferenceString(used->_decorated.seeAlso(), preferredLang) << std::endl;
    }
    if ( !used->_decorated.isDefinedBy().empty() ) {
        indent(ofs, numIndent) << " * @see " << langPreferenceString(used->_decorated.isDefinedBy(), preferredLang) << std::endl;
    }

    boost::tokenizer<boost::char_separator<char> > lines(additionalComment, NEWLINE);
    for (const std::string& line : lines) {
        if ( !line.empty() ) {
            indent(ofs, numIndent) << " * " << line << std::endl;
        }
    }

    indent(ofs, numIndent) << " */" << std::endl;
}

void RdfsEntity::generatePropertyComment(std::ostream& ofs, const Klass& onClass, const std::string& methodName, unsigned int numIndent, const std::string& additionalComment, const RdfsEntity *alternate) const {
    std::string addendum = std::string("This method ") + onClass.genCppNameWithNamespace() + "::" + methodName + " uses " + _decorated.rdfname() + " rdfs/owl property";
    generateComment(ofs, numIndent, additionalComment + "\n" + addendum, alternate);
}

}
}

