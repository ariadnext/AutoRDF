#ifndef AUTORDF_PROPERTYVALUE_H
#define AUTORDF_PROPERTYVALUE_H

#include <string>

#include <autordf/cvt/Cvt.h>

#include <autordf/autordf_export.h>

namespace autordf {

class I18String;

/**
 * Stores a Value (aka Literal) of a Web Semantic Resource
 */
class PropertyValue : public std::string {
public:
    /**
     * Builds empty value
     */
    PropertyValue() {}

    /**
     * Builds from a string literal
     *
     * @param rawValue Literal value
     */
    PropertyValue(const char *rawValue) : std::string(rawValue) {}

    /**
     * Builds from a string literal
     *
     * @param rawValue Literal value
     * @param lang optional xml:lang for this literal property
     * @param dataTypeIri optional literal data type
     */
    PropertyValue(const std::string& rawValue, const std::string& lang = "", const std::string& dataTypeIri = "")
            : std::string(rawValue), _lang(lang), _dataTypeIri(dataTypeIri) {}

    /**
     * Builds from a string literal
     *
     * @param rawValue Literal value
     * @param lang optional xml:lang for this literal property
     * @param dataTypeIri optional literal data type
     */
    PropertyValue(const std::string& rawValue, const char* lang, const char* dataTypeIri)
            : std::string(rawValue), _lang(lang ? lang : ""), _dataTypeIri(dataTypeIri ? dataTypeIri : "") {}

    /**
    * Builds from an internationalized string
    *
    * @param value internationalized string value
    */
    PropertyValue(const autordf::I18String& value);

    /**
     * @return the literal data type if it is set, blank string otherwise
     * Does makes sense only in case this property holds a literal
     */
    AUTORDF_EXPORT const std::string& dataTypeIri() const;

    /**
     * @return the literal lang if it is set, blank string otherwise
     * Does makes sense only in case this property holds a literal
     */
    AUTORDF_EXPORT const std::string& lang() const;

    /**
     * Set Literal data type
     * Does makes sense only in case this property holds a literal
     */
    AUTORDF_EXPORT void setDataTypeIri(const std::string& dataTypeIri);

    /**
     * Set literal language.
     * @see http://www.ietf.org/rfc/rfc4646.txt
     * Does makes sense only in case this property holds a literal
     */
    AUTORDF_EXPORT void setLang(const std::string& lang);

    /**
     * Print content to stream for debugging purpose
     */
    AUTORDF_EXPORT std::ostream& printStream(std::ostream& os);

    /**
     * Writes the Value, transtyping it from C++ to rdf type
     * @throw DataConvertionFailure
     */
    template<cvt::RdfTypeEnum rdfType, typename T> PropertyValue& set(const T& val) {
        assign(cvt::toRdf<T, rdfType>::val(val));
        return *this;
    };

    /**
     * Gets the Value, transtyping it from Rdf to C++ type
     * @throw DataConvertionFailure
     */
    template<cvt::RdfTypeEnum rdfType, typename T> T get() const {
        return cvt::toCpp<T, rdfType>::val(*this);
    };

    /**
     * Comparison operator
     */
    bool operator==(const PropertyValue& val) const;
    bool operator!=(const PropertyValue& val) const;
    bool operator==(const std::string& s) const;
    bool operator!=(const std::string& s) const;
    bool operator==(const char* s) const;
    bool operator!=(const char* s) const;

private:
    std::string _lang;
    std::string _dataTypeIri;

    /**
     * Gets the data type of the Property Value
     * @return
     */
    std::string explicitDataType() const;
};

/**
 * Template specialization for I18String
 */
template<> inline PropertyValue& PropertyValue::set<cvt::RdfTypeEnum::rdf_langString, I18String>(const I18String& val) {
    assign(cvt::toRdf<autordf::I18String, cvt::RdfTypeEnum::rdf_langString>::val(val));
    _lang = val.lang();
    return *this;
}

}

#endif //AUTORDF_PROPERTYVALUE_H
