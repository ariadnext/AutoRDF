#include "autordf/I18String.h"
#include "autordf/Exception.h"
#include "autordf/PropertyValue.h"

namespace autordf {

I18String::I18String(const std::string& s, const std::string& lang)
        : std::string(s), _lang(lang) {
    if (lang.empty()) {
        throw DataConvertionFailure("During conversion of " + s + " to I18String: no lang set");
    }
}

I18String::I18String(const PropertyValue& value)
        : std::string(value), _lang(value.lang()) {
    if (value.lang().empty()) {
        throw DataConvertionFailure("During conversion of " + value + " to I18String: no lang set");
    }
}

const std::string& I18String::lang() const {
    return _lang;
}

void I18String::setLang(const std::string& lang) {
    _lang = lang;
}

bool I18String::operator==(const I18String& s) const {
    return std::string(*this) == std::string(s) && _lang == s.lang();
}

bool I18String::operator!=(const I18String& s) const {
    return !(*this == s);
}
}
