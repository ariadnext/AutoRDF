//
// Created by qdauchy on 07/09/2022.
//

#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <autordf/codegen/UtilsCommon.h>

namespace autordf {
namespace codegen {
void createDirectory(const std::string& relativeDirName) {
    if (!relativeDirName.empty() && relativeDirName != ".") {
#       ifdef WIN32
        if (ULONG len = GetFullPathNameW(CA2W(relativeDirName.c_str()), 0, 0, 0)) {
            PWSTR buf = (PWSTR)calloc((4 + len + 1), sizeof(WCHAR));
            buf[0] = L'\\', buf[1] = L'\\',  buf[2] = L'?', buf[3] = L'\\';
            ULONG len2 = GetFullPathNameW(CA2W(relativeDirName.c_str()), len + 1, buf + 4, NULL);
            if (len - 1 == len2) {
                if (CreateDirectoryW(buf, NULL) == 0) {
                    if (GetLastError() != ERROR_ALREADY_EXISTS) {
                        free(buf);
                        std::stringstream ss;
                        ss << "Error while creating output directory: " << GetLastError();
                        throw std::runtime_error(ss.str());
                    }
                }
                free(buf);
            } else {
                free(buf);
                std::stringstream ss;
                ss << "Internal error getting full path from " << relativeDirName << ": GetFullPathNameW len " << len << " diff " << len2;
                throw std::runtime_error(ss.str());
            }
        } else {
            std::stringstream ss;
            ss << "Unable to get full path from " << relativeDirName;
            throw std::runtime_error(ss.str());
        }
#       else
        if (mkdir(relativeDirName.c_str(), S_IRWXG | S_IRWXU | S_IRWXO) != 0) {
            if (errno != EEXIST) {
                std::stringstream ss;
                ss << "Error while creating output directory: " << ::strerror(errno);
                throw std::runtime_error(ss.str());
            }
        }
#       endif
    }
}

void createFile(const std::string& fileName, std::ofstream *ofs, bool append) {
    ofs->open(fileName, append ? std::ios::app : std::ios::out);
    if (!ofs->is_open()) {
        throw std::runtime_error("Unable to open " + fileName + " file");
    }
}
}
}