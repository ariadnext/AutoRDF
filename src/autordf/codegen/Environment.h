#pragma once

#include <string>
#include <vector>

#include "autordf/PropertyValue.h"

namespace autordf {
namespace codegen {
/**
 * This class stores execution parameters as well as offering utility function to interact with the system
 */
class Environment {
public:
    static const boost::char_separator<char> RDF_COMMENT_NEWLINE;

    /**
     * Stores the directory where the find the template files
     */
    static std::string tpldir;

    /**
     * Stores the global namespace for the generated code
     */
    static std::string namespace_;

    /**
     * Stores the directory where the generated code will be placed
     */
    static std::string outdir;

    /**
     * A list of languages for the generated code documentation ordered by preference
     */
    static std::vector<std::string> preferredLang;

    /**
     * Tells if the command should print additional debug text to the shell
     */
    static bool verbose;

    /**
     * Creates a folder with a relative path to the out directory
     *
     * @param relativeDirName a relative path to the out directory
     */
    static void createOutDirectory(const std::string& relativeDirName);

    /**
     * Creates a folder with a path relative to the current directory
     *
     * @param relativeDirName a path relative to the current directory
     */
    static void createDirectory(const std::string& relativeDirName);

    /**
     * Creates a file with a path relative to the current directory
     *
     * @param fileName a path relative to the current directory
     * @param ofs a handle to write into the newly created file
     * @param append if true, the content of an existing file will be kept and the new content written after otherwise
     *               the content will be overwritten
     */
    static void createFile(const std::string& fileName, std::ofstream& ofs, bool append = false);

    /**
     * Returns the best property translation from a list of values using preferredLang
     *
     * @param values a list of values for the property
     *
     * @return the best property translation, or returns first non empty translation if not found, or an empty string if there is no values
     */
    static std::string propertyI18(const std::vector<PropertyValue>& values);
};
}
}
