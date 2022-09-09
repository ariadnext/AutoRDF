#include "Utils.h"

#include <algorithm>

#ifdef WIN32
#include <Windows.h>
#include <atlbase.h>
#endif

#include <iostream>
#include <cstring>
#include <sstream>
#include <fstream>

namespace autordf {
namespace codegen {

std::ostream& indent(std::ostream& os, unsigned int numIndent) {
    for (unsigned int i = 0; i < numIndent * 4; ++i) {
        os << ' ';
    }
    return os;
}

void startInternal(std::ostream& os, int numIndent) {
    indent(os, numIndent);
    os << "/** @cond internal */" << std::endl;
}

void stopInternal(std::ostream& os, int numIndent) {
    indent(os, numIndent);
    os << "/** @endcond internal */" << std::endl;
}

void addBoilerPlate(std::ofstream& ofs) {
    ofs << "// This is auto generated code by AutoRDF, do not edit !" << std::endl;
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

