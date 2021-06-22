#include "autordf/I18StringVector.h"

#include <algorithm>
#include <ostream>

namespace autordf {

I18StringVector::I18StringVector(const std::vector<PropertyValue>& values) {
    for (auto const& value : values) {
        if (value.lang().empty()) {
            throw DataConvertionFailure("During conversion of " + value + " to I18String: no lang set");
        }
        emplace_back(value);
    }
}

std::vector<PropertyValue> I18StringVector::asPropertyValueVector() const {
    std::vector<PropertyValue> res;
    for (auto const& s : *this) {
        res.emplace_back(s);
    }
    return res;
}

std::shared_ptr<I18String> I18StringVector::langOptional(const std::string& lang) const {
    for (const I18String& pv : *this) {
        if ( pv.lang() == lang ) {
            return std::make_shared<I18String>(pv);
        }
    }
    return nullptr;
}

std::shared_ptr<I18String> I18StringVector::langPreferenceOptional(const std::vector<std::string>& preferredLang) const {
    auto bestLang = preferredLang.end();
    auto bestIt = this->begin();
    for (auto it = this->begin(); it != this->end(); it++) {
        auto matchLang = std::find(preferredLang.begin(), preferredLang.end(), it->lang());
        if (matchLang < bestLang) {
            bestIt = it;
            bestLang = matchLang;
        }
    }
    if (bestIt != this->end()) {
        return std::make_shared<I18String>(*bestIt);
    }
    return nullptr;
}

std::string I18StringVector::langPreferenceString(const std::vector<std::string>& preferredLang) const {
    auto value = langPreferenceOptional(preferredLang);
    return value? std::string(*value) : "";
}

std::ostream& operator<<(std::ostream& os, const I18StringVector& vv) {
    for ( auto it = vv.begin(); it != vv.end(); ++it ) {
        if ( it != vv.begin() ) {
            os << ", ";
        }
        os << *it;
    }
    return os;
}

}