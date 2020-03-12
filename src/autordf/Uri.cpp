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

std::string Uri::prettyName(PrettyFormat format) const {
    std::string pretty = substr(find_last_of("#/") + 1);
    switch(format) {
        case PrettyFormat::UNMODIFIED:
            break;
        case PrettyFormat::LOWERCASEFIRST:
            pretty[0] = ::tolower(pretty[0]);
            break;
        case PrettyFormat::UPPERCASEFIRST:
            pretty[0] = ::toupper(pretty[0]);
            break;
    }
    return pretty;
}

std::string Uri::prefixPart() const {
    return substr(0, find_last_of("#/"));
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
