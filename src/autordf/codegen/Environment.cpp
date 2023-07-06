#include "Environment.h"

#include <sys/stat.h>
#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <fstream>
#include <sstream>

#include "autordf/I18StringVector.h"

namespace autordf {
namespace codegen {
const boost::char_separator<char> Environment::RDF_COMMENT_NEWLINE = boost::char_separator<char>{"\r\n"};
std::string Environment::tpldir = "./templates";
std::string Environment::outdir = ".";
std::string Environment::namespace_ = "";
std::vector<std::string> Environment::preferredLang = {"en", "fr"};
bool Environment::verbose = false;

void Environment::createOutDirectory(const std::string& relativeDirName) {
    if (!relativeDirName.empty() && relativeDirName != ".") {
        if (mkdir((Environment::outdir + '/' + relativeDirName).c_str(), S_IRWXG | S_IRWXU | S_IRWXO) != 0) {
            if (errno != EEXIST) {
                std::stringstream ss;
                ss << "Error while creating output directory: " << Environment::outdir + '/' + relativeDirName << ::strerror(errno);
                throw std::runtime_error(ss.str());
            }
        }
    }
}

void Environment::createDirectory(const std::string& relativeDirName) {
    if (!relativeDirName.empty() && relativeDirName != ".") {
        if (mkdir(relativeDirName.c_str(), S_IRWXG | S_IRWXU | S_IRWXO) != 0) {
            if (errno != EEXIST) {
                std::stringstream ss;
                ss << "Error while creating output directory: " << Environment::outdir + '/' + relativeDirName << ::strerror(errno);
                throw std::runtime_error(ss.str());
            }
        }
    }
}

void Environment::createFile(const std::string& fileName, std::ofstream& ofs, bool append) {
    ofs.open(Environment::outdir +'/'+ fileName, append ? std::ios::app : std::ios::out);
    // ofs.open(fileName, append ? std::ios::app : std::ios::out);
    if (!ofs.is_open()) {
        throw std::runtime_error("Unable to open " + fileName + " file");
    }
}

std::string Environment::propertyI18(const std::vector<PropertyValue>& values) {
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
}
}
