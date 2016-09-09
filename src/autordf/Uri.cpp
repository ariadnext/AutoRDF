#include "autordf/Uri.h"

#include <string>

#include <autordf/Model.h>

namespace autordf {

std::string Uri::localPart() const {
    std::string::size_type mark = find_last_of("#/");
    if ( length() > (mark + 1) ) {
        return substr(mark + 1);
    } else {
        return *this;
    }
}

std::string Uri::prettyName(bool uppercaseFirst) const {
    std::string pretty = substr(find_last_of("#/") + 1);
    if ( uppercaseFirst ) {
        pretty[0] = ::toupper(pretty[0]);
    }
    return pretty;
}

std::string Uri::QName(const Model *model) const {
    if (model) {
        std::string bestValue;
        std::string bestPrefix;
        for (auto const& p : model->namespacesPrefixes()) {
            auto value = p.second;
            if ( (find(value) == 0) && (value.length() > bestPrefix.length()) ) {
                bestValue = value;
                bestPrefix = p.first;
            }
        }
        if ( (find(model->baseUri()) == 0) && (model->baseUri().length() > bestPrefix.length()) ) {
            bestValue = model->baseUri();
            bestPrefix = "";
        }
        std::string ret = bestPrefix;
        if ( bestPrefix.length() ) {
            ret.push_back(':');
        }
        ret.append(substr(bestValue.length()));
        return ret;
    }
    else {
        return *this;
    }
}


}
