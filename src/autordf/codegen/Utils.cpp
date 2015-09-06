#include "Utils.h"

#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

#include <autordf/Model.h>

namespace autordf {
namespace codegen {

std::ostream& indent(std::ostream& os, int numIndent) {
    for (unsigned int i = 0; i < numIndent * 4; ++i) {
        os << ' ';
    }
    return os;
}

void addBoilerPlate(std::ofstream& ofs) {
    ofs << "// This is auto generated code by AutoRDF, do not edit !" << std::endl;
}

void createDirectory(const std::string& relativeDirName) {
    if ( !relativeDirName.empty() && relativeDirName != "." ) {
        if ( ::mkdir(relativeDirName.c_str(), S_IRWXG | S_IRWXU | S_IRWXO) != 0 ) {
            if ( errno != EEXIST ) {
                std::stringstream ss;
                ss << "Error while creating output directory: " << ::strerror(errno);
                throw std::runtime_error(ss.str());
            }
        }
    }
}

void createFile(const std::string& fileName, std::ofstream *ofs) {
    ofs->open(fileName);
    if (!ofs->is_open()) {
        throw std::runtime_error("Unable to open " + fileName + " file");
    }
}

// Checks an returns if available registered prefix for IRI
std::string rdfPrefix(const std::string& rdfiri, const Model *model) {
    for ( const std::pair<std::string, std::string>& prefixMapItem : model->namespacesPrefixes() ) {
        const std::string& iri = prefixMapItem.second;
        if ( rdfiri.substr(0, iri.length()) == iri ) {
            return prefixMapItem.first;
        }
    }
    return "";
}

void generateCodeProtectorBegin(std::ofstream& ofs, const std::string& cppNameSpace, const std::string& cppName) {
    std::string upperCppNameSpace = cppNameSpace;
    std::transform(upperCppNameSpace.begin(), upperCppNameSpace.end(), upperCppNameSpace.begin(), ::toupper);
    std::string upperClassName = cppName;
    std::transform(upperClassName.begin(), upperClassName.end(), upperClassName.begin(), ::toupper);

    std::string protector = upperCppNameSpace + "_" + upperClassName;

    ofs << "#ifndef " << protector << std::endl;
    ofs << "#define " << protector << std::endl;
    ofs << std::endl;
    addBoilerPlate(ofs);
    ofs << std::endl;
}

void generateCodeProtectorEnd(std::ofstream& ofs, const std::string& cppNameSpace, const std::string& cppName) {
    std::string upperCppNameSpace = cppNameSpace;
    std::transform(upperCppNameSpace.begin(), upperCppNameSpace.end(), upperCppNameSpace.begin(), ::toupper);
    std::string upperClassName = cppName;
    std::transform(upperClassName.begin(), upperClassName.end(), upperClassName.begin(), ::toupper);

    std::string protector = upperCppNameSpace + "_" + upperClassName;

    ofs << "#endif // " <<  protector << std::endl;
}

}
}

