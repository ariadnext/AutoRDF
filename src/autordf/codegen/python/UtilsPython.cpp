#include "UtilsPython.h"

#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

#ifdef WIN32
#include <Windows.h>
#include <atlbase.h>
#endif

#include <iostream>
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
    os << R"("""@cond internal""")" << std::endl;
}

void stopInternal(std::ostream& os, int numIndent) {
    indent(os, numIndent);
    os << R"("""@endcond internal""")" << std::endl;
}

void addBoilerPlate(std::ofstream& ofs) {
    ofs << R"("""This is auto generated code by AutoRDF, do not edit !""")" << std::endl;
}
}
}

