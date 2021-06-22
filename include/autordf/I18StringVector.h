#pragma once

#include <vector>

#include "I18String.h"
#include "PropertyValue.h"

#include <autordf/autordf_export.h>

namespace autordf {

/**
 * Stores a list of internationalized string
 */
class I18StringVector : public std::vector<I18String> {
public:
    using std::vector<I18String>::vector;

    AUTORDF_EXPORT  I18StringVector()  : vector() {};

    /**
     * Build a I18String vector from a list of I18String
     *
     * @param values
     */
    AUTORDF_EXPORT I18StringVector(const std::vector<I18String>& values)  : vector(values) {};

    /**
     * Build a I18String vector from a list of property values
     * @param values
     *
     * @throw DataConvertionFailure if property values are not I18String
     */
    AUTORDF_EXPORT explicit I18StringVector(const std::vector<PropertyValue>& values);

    /**
     * Convert I18String vector to a vector of PropertyValue
     * @return
     */
    AUTORDF_EXPORT std::vector<PropertyValue> asPropertyValueVector() const;

    /**
     * Returns the I18String from the whose language is identified by lang
     * If not found, returns nullptr
     * @param lang
     * @return
     */
    AUTORDF_EXPORT std::shared_ptr<I18String> langOptional(const std::string& lang) const;

    /**
     * Returns the I18String with the best lang from the language list preferredLang (meaning first element of the list)
     * If not found, returns first non empty I18String
     * If vector is empty return nullptr
     * @param preferredLang list of preferred language, order is important
     * @return
     */
    AUTORDF_EXPORT std::shared_ptr<I18String> langPreferenceOptional(const std::vector<std::string>& preferredLang) const;

    /**
     * Same as previous function but returns an empty string if no value is found
     */
    AUTORDF_EXPORT std::string langPreferenceString(const std::vector<std::string>& preferredLang) const;
};

std::ostream& operator<<(std::ostream& os, const I18StringVector& v);

}

class I18StringVector {

};
