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

    template<typename T> void set(const T& val);
    template<typename T> T get() const;
};

}

#endif //AUTORDF_PROPERTYVALUE_H
