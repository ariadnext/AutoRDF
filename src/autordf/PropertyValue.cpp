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

} //namespace autordf

bool
operator==(const autordf::PropertyValue& __lhs,
           const autordf::PropertyValue& __rhs)
{
    return static_cast<std::string>(__lhs) == static_cast<std::string>(__rhs) &&
           __lhs.lang() == __rhs.lang() &&
           __lhs.dataTypeIri() == __rhs.dataTypeIri();
}

bool
operator==(const autordf::PropertyValue& __lhs,
           const std::string& __rhs)
{
    return static_cast<std::string>(__lhs) == __rhs &&
            __lhs.dataTypeIri() == "http://www.w3.org/2001/XMLSchema#string";
}

bool
operator==(const std::string& __lhs,
           const autordf::PropertyValue& __rhs)
{
    return __lhs == static_cast<std::string>(__rhs) &&
            __rhs.dataTypeIri() == "http://www.w3.org/2001/XMLSchema#string";
}

bool
operator==(const autordf::PropertyValue& __lhs,
           const char* __rhs)
{
    return __rhs &&
        0 == strcmp(__lhs.c_str(), __rhs) &&
        __lhs.dataTypeIri() == "http://www.w3.org/2001/XMLSchema#string";
}

bool
operator==(const char* __lhs,
           const autordf::PropertyValue& __rhs)
{
    return __lhs &&
        0 == strcmp(__lhs, __rhs.c_str()) &&
        __rhs.dataTypeIri() == "http://www.w3.org/2001/XMLSchema#string";
}

bool
operator==(const autordf::PropertyValue& __lhs,
           const std::int16_t& __rhs){
    return std::stoi(__lhs) == __rhs &&
        ( __lhs.dataTypeIri() == "http://www.w3.org/2001/XMLSchema#short" ||
        __lhs.dataTypeIri() == "http://www.w3.org/2001/XMLSchema#integer" );
}

bool
operator==(const std::int16_t& __lhs,
           const autordf::PropertyValue& __rhs){
    return __lhs == std::stoi(__rhs)  &&
        (__rhs.dataTypeIri() == "http://www.w3.org/2001/XMLSchema#short" ||
        __rhs.dataTypeIri() == "http://www.w3.org/2001/XMLSchema#integer");
}

bool
operator==(const autordf::PropertyValue& __lhs,
           const std::uint16_t& __rhs){
    return std::stoul(__lhs) == __rhs &&
        ( __lhs.dataTypeIri() == "http://www.w3.org/2001/XMLSchema#unsignedshort" ||
        __lhs.dataTypeIri() == "http://www.w3.org/2001/XMLSchema#integer" );
}

bool
operator==(const std::uint16_t& __lhs,
           const autordf::PropertyValue& __rhs){
    return __lhs == std::stoul(__rhs) &&
         (__rhs.dataTypeIri() == "http://www.w3.org/2001/XMLSchema#unsignedshort" ||
         __rhs.dataTypeIri() == "http://www.w3.org/2001/XMLSchema#integer");
}

bool
operator==(const autordf::PropertyValue& __lhs,
           const std::int32_t& __rhs){
    return std::stoi(__lhs) == __rhs &&
        ( __lhs.dataTypeIri() == "http://www.w3.org/2001/XMLSchema#int" ||
        __lhs.dataTypeIri() == "http://www.w3.org/2001/XMLSchema#integer" );
}

bool
operator==(const std::int32_t& __lhs,
           const autordf::PropertyValue& __rhs){
    return __lhs == std::stoi(__rhs)  &&
        (__rhs.dataTypeIri() == "http://www.w3.org/2001/XMLSchema#int" ||
        __rhs.dataTypeIri() == "http://www.w3.org/2001/XMLSchema#integer");
}

bool
operator==(const autordf::PropertyValue& __lhs,
           const std::uint32_t& __rhs){
    return std::stoul(__lhs) == __rhs &&
        ( __lhs.dataTypeIri() == "http://www.w3.org/2001/XMLSchema#unsignedint" ||
        __lhs.dataTypeIri() == "http://www.w3.org/2001/XMLSchema#integer" );
}

bool
operator==(const std::uint32_t& __lhs,
           const autordf::PropertyValue& __rhs){
    return __lhs == std::stoul(__rhs) &&
        (__rhs.dataTypeIri() == "http://www.w3.org/2001/XMLSchema#unsignedint" ||
        __rhs.dataTypeIri() == "http://www.w3.org/2001/XMLSchema#integer");
}