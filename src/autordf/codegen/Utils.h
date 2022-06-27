#ifndef AUTORDF_CODEGEN_UTILS_H
#define AUTORDF_CODEGEN_UTILS_H

#include <string>
#include <iosfwd>

namespace autordf {

class Model;

namespace codegen {

std::ostream& indent(std::ostream& os, unsigned int numIndent);

void startInternal(std::ostream& os, int numIndent = 0);
void stopInternal(std::ostream& os, int numIndent = 0);

void addBoilerPlate(std::ofstream& ofs);
void generateCodeProtectorBegin(std::ofstream& ofs, const std::string& cppNameSpace, const std::string& cppName);
void generateCodeProtectorEnd(std::ofstream& ofs, const std::string& cppNameSpace, const std::string& cppName);

void createDirectory(const std::string& relativeDirName);

void createFile(const std::string& fileName, std::ofstream *ofs);
}
}


#endif //AUTORDF_CODEGEN_UTILS_H
