#pragma once

#include <string>
#include <vector>
#include <memory>

namespace autordf {

class  PropertyValue;

/**
 * Stores an internationalized string
 */
class I18String : public std::string {
public:
    /**
     * Builds empty value
     */
    I18String() {}

    /**
     * Builds from a string and a language
     *
     * @param s string value
     * @param lang for this string
     *
     * @throw DataConvertionFailure if lang is empty
     */
    I18String(const std::string& s, const std::string& lang);

    /**
     * Builds from a property value
     *
     * @param value
     *
     * @throw DataConvertionFailure if property value lang is empty
     */
    explicit I18String(const PropertyValue& value);

    /**
    * @return the string lang
    */
    const std::string& lang() const;

    /**
     * Set string language
     */
    void setLang(const std::string& lang);

    /**
     * Comparison operator
     */
    bool operator==(const I18String& s) const;
    bool operator!=(const I18String& s) const;
private:
    std::string _lang;
};
}
