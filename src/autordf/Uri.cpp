#include "autordf/Uri.h"

#include <string>

namespace autordf {

std::string Uri::localPart() const {
    std::string::size_type mark = find_last_of("#:/");
    if ( length() > (mark + 1) ) {
        return substr(mark + 1);
    } else {
        return *this;
    }
}

}
