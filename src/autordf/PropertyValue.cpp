#include <autordf/PropertyValue.h>
#include <autordf/cvt/RdfTypeEnum.h>
#include <autordf/I18String.h>

namespace autordf {

PropertyValue::PropertyValue(const autordf::I18String& value)
        : std::string(value), _lang(value.lang()) {}

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

bool PropertyValue::operator==(const PropertyValue& s) const {
    bool lexicalFormEquality = std::string(*this) == std::string(s);
    bool dataTypeEquality = explicitDataType() == s.explicitDataType();
    bool langTagEquality = _lang == s.lang();
    return lexicalFormEquality && dataTypeEquality && langTagEquality;
}

bool PropertyValue::operator!=(const PropertyValue& val) const  {
    return !(*this == val);
}

bool PropertyValue::operator==(const std::string& s) const {
    return std::string(*this) == s;
}

bool PropertyValue::operator!=(const std::string& s) const {
    return !(*this == s);
}

bool PropertyValue::operator==(const char *s) const {
    return std::string(*this) == s;
}

bool PropertyValue::operator!=(const char *s) const {
    return !(*this == s);
}

std::string PropertyValue::explicitDataType() const {
    if (!_dataTypeIri.empty()) {
        return _dataTypeIri;
    }
    if (_lang.empty()) {
        return cvt::rdfTypeIri(cvt::RdfTypeEnum::xsd_string);
    }
    return cvt::rdfTypeIri(cvt::RdfTypeEnum::rdf_langString);
}
}
