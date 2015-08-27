#ifndef AUTORDF_PROPERTYVALUE_H
#define AUTORDF_PROPERTYVALUE_H

#include <string>

#include <autordf/cvt/Cvt.h>

namespace autordf {

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
     * @param raw Literal value
     */
    PropertyValue(const char *rawValue) : std::string(rawValue) {}

    /**
     * Builds from a string literal
     *
     * @param raw Literal value
     */
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
