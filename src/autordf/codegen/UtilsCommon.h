//
// Created by qdauchy on 07/09/2022.
//

#ifndef AUTORDF_ON_IRMA_AXTBUILDER_UTILSCOMMON_H
#define AUTORDF_ON_IRMA_AXTBUILDER_UTILSCOMMON_H

#include <string>

namespace autordf {
namespace codegen {
void createDirectory(const std::string& relativeDirName);
void createFile(const std::string& fileName, std::ofstream *ofs, bool append = false);
}
}
#endif //AUTORDF_ON_IRMA_AXTBUILDER_UTILSCOMMON_H
