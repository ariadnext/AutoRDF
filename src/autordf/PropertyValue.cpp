#include <autordf/PropertyValue.h>

namespace autordf {

const std::string& PropertyValue::dataTypeIri() const {
    return _dataTypeIri;
}

const std::string& PropertyValue::lang() const {
    return _lang;
}

void PropertyValue::setDataTypeIri(const std::string& dataTypeIri) {
    _dataTypeIri = dataTypeIri;
}

void PropertyValue::setLang(const std::string& lang) {
    _lang = lang;
}

std::ostream& operator<<(std::ostream& os, const PropertyValue& val) {
    os << static_cast<std::string>(val);
    if ( val.dataTypeIri().length() ) {
        os << "^^<" << val.dataTypeIri() << ">";
    }
    if ( val.lang().length() ) {
        os << "@" << val.lang();
    }
    return os;
}
}
