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

std::ostream& PropertyValue::printStream(std::ostream& os) {
    os << static_cast<std::string>(*this);
    if ( dataTypeIri().length() ) {
        os << "^^<" << dataTypeIri() << ">";
    }
    if ( lang().length() ) {
        os << "@" << lang();
    }
    return os;
}
}
