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

bool
operator==(const autordf::PropertyValue& __lhs,
           const autordf::PropertyValue& __rhs)
{
    return static_cast<std::string>(__lhs) == static_cast<std::string>(__rhs) &&
           __lhs.lang() == __rhs.lang() &&
           __lhs.dataTypeIri() == __rhs.dataTypeIri();
}

bool
operator!=(const autordf::PropertyValue& __lhs,
           const autordf::PropertyValue& __rhs)
{
    return static_cast<std::string>(__lhs) != static_cast<std::string>(__rhs) ||
           __lhs.lang() != __rhs.lang() ||
           __lhs.dataTypeIri() != __rhs.dataTypeIri();
}


bool
operator==(const autordf::PropertyValue& __lhs,
           const std::string& __rhs)
{
    return static_cast<std::string>(__lhs) == __rhs &&
            __lhs.dataTypeIri() == autordf::datatype::DATATYPE_STRING;
}

bool
operator!=(const autordf::PropertyValue& __lhs,
           const std::string& __rhs)
{
    return static_cast<std::string>(__lhs) != __rhs ||
           __lhs.dataTypeIri() != autordf::datatype::DATATYPE_STRING;
}

bool
operator==(const std::string& __lhs,
           const autordf::PropertyValue& __rhs)
{
    return __lhs == static_cast<std::string>(__rhs) &&
            __rhs.dataTypeIri() == autordf::datatype::DATATYPE_STRING;
}

bool
operator!=(const std::string& __lhs,
           const autordf::PropertyValue& __rhs)
{
    return __lhs != static_cast<std::string>(__rhs) ||
           __rhs.dataTypeIri() != autordf::datatype::DATATYPE_STRING;
}
