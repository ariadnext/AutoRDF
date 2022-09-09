#include "RdfsEntityPython.h"

#include <ostream>
#include <stdexcept>

#include <autordf/I18StringVector.h>
#include <autordf/ontology/Ontology.h>
#include <autordf/Model.h>
#include <autordf/codegen/python/KlassPython.h>
#include "UtilsPython.h"

#include <boost/tokenizer.hpp>

namespace autordf {
namespace codegen {

std::string RdfsEntityPython::outdir = ".";
std::vector<std::string> RdfsEntityPython::preferredLang = {"en", "fr"};

std::string RdfsEntityPython::genModuleInternal(const char *sep) const {
    std::string prefix = _decorated.ontology()->model()->iriPrefix(_decorated.rdfname());
    if ( !prefix.empty() ) {
        if ( outdir != "." && sep) {
            return outdir + sep + prefix;
        } else {
            return prefix;
        }
    } else {
        throw std::runtime_error("No prefix found for " + _decorated.rdfname() + " RDF resource, unable to use it as Python namespace");
    }
}

std::string RdfsEntityPython::genModule() const {
    return genModuleInternal(nullptr);
}

std::string RdfsEntityPython::genModuleInclusionPath() const {
    return genModuleInternal("/");
}

std::string RdfsEntityPython::genModuleFullyQualified() const {
    return genModuleInternal(".");
}

std::string RdfsEntityPython::genNameWithModule(bool interfaceMode) const {
    return genModuleFullyQualified() + "." + (interfaceMode ? "I" : "") + _decorated.prettyIRIName();
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

void RdfsEntityPython::generateComment(std::ostream& ofs, unsigned int numIndent, const std::string& additionalComment, const RdfsEntityPython *alternate) const {
    static const boost::char_separator<char> NEWLINE("\r\n");
    auto decor = decorated();
    if ( decor.label().empty() && decor.comment().empty() ) {
        if ( alternate ) {
            decor = alternate->decorated();
        }
    }
    indent(ofs, numIndent) << "\"\"\"" << std::endl;
    if ( !decor.label().empty() ) {
        indent(ofs, numIndent) << "@brief " << langPreferenceString(decor.label(), preferredLang) << std::endl;
    }
    if ( !decor.comment().empty() ) {
        // windows lib needs a local reference for iterator to work
        // was getting "cannot dereference string iterator because the iterator ..."
        const std::string& comment = langPreferenceString(decor.comment(), preferredLang);
        boost::tokenizer<boost::char_separator<char> > lines(comment, NEWLINE);
        for (const std::string& line : lines) {
            if ( !line.empty() ) {
                indent(ofs, numIndent) << line << std::endl;
            }
        }
    }

    if ( !decor.seeAlso().empty() ) {
        indent(ofs, numIndent) << "@see " << langPreferenceString(decor.seeAlso(), preferredLang) << std::endl;
    }
    if ( !decor.isDefinedBy().empty() ) {
        indent(ofs, numIndent) << "@see " << langPreferenceString(decor.isDefinedBy(), preferredLang) << std::endl;
    }

    boost::tokenizer<boost::char_separator<char> > lines(additionalComment, NEWLINE);
    for (const std::string& line : lines) {
        if ( !line.empty() ) {
            indent(ofs, numIndent) << line << std::endl;
        }
    }

    indent(ofs, numIndent) << "\"\"\"" << std::endl;
}

void RdfsEntityPython::generatePropertyComment(std::ostream& ofs, const KlassPython& onClass, const std::string& methodName, unsigned int numIndent, const std::string& additionalComment, const RdfsEntityPython *alternate) const {
    std::string addendum = std::string("This method ") + onClass.genNameWithModule() + "." + methodName + " uses " + _decorated.rdfname() + " rdfs/owl property";
    generateComment(ofs, numIndent, additionalComment + "\n" + addendum, alternate);
}

}
}

