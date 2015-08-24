#ifndef AUTORDF_PROPERTYVALUE_H
#define AUTORDF_PROPERTYVALUE_H

#include <string>

#include <autordf/cvt/Cvt.h>

namespace autordf {

class PropertyValue : public std::string {
public:
    PropertyValue() {}
    PropertyValue(const char *rawValue) : std::string(rawValue) {}
    PropertyValue(const std::string& rawValue) : std::string(rawValue) {}

    /**
     * Writes the Value, transtyping it from C++ to rdf type
     * @throw DataConvertionFailure
     */
    template<cvt::RdfTypeEnum rdfType, typename T> void set(const T& val) {
        assign(cvt::toRdf<T, rdfType>::val(val));
    };

    /**
     * Gets the Value, transtyping it from Rdf to C++ type
     * @throw DataConvertionFailure
     */
    template<cvt::RdfTypeEnum rdfType, typename T> T get() const {
        return cvt::toCpp<T, rdfType>::val(*this);
    };
};

}

#endif //AUTORDF_PROPERTYVALUE_H
