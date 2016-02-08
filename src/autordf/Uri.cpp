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

std::string Uri::prettyName(bool uppercaseFirst) const {
    std::string pretty = substr(find_last_of("/#:") + 1);
    if ( uppercaseFirst ) {
        pretty[0] = ::toupper(pretty[0]);
    }
    return pretty;
}

}
