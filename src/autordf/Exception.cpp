#include "autordf/Exception.h"

namespace autordf {

Exception::Exception(const std::string& val) {
    _description = "AutoRdf " + val;
}

Exception::~Exception() throw() {
}

const char * Exception::what() const throw() {
    return _description.c_str();
}

}
