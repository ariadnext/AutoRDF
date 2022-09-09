#ifndef AUTORDF_CODEGEN_UTILS_H
#define AUTORDF_CODEGEN_UTILS_H

#include <string>
#include <iosfwd>

namespace autordf {

namespace codegen {

std::ostream& indent(std::ostream& os, unsigned int numIndent);

void startInternal(std::ostream& os, int numIndent = 0);
void stopInternal(std::ostream& os, int numIndent = 0);

void addBoilerPlate(std::ofstream& ofs);
}
}


#endif //AUTORDF_CODEGEN_UTILS_H
