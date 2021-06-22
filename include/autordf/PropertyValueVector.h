#ifndef AUTORDF_PROPERTYVALUELIST_H
#define AUTORDF_PROPERTYVALUELIST_H

#include <vector>
#include <memory>
#include <iosfwd>

#include <autordf/PropertyValue.h>

#include <autordf/autordf_export.h>

namespace autordf {

/**
 * Stores a List of values(aka Literal) of a Web Semantic Resource
 */
class PropertyValueVector : public std::vector<PropertyValue> {
public:
    using std::vector<PropertyValue>::vector;

    /**
     * Returns the Value from the whose language is idendified by lang
     * If not found, returns nullptr
     */
    AUTORDF_EXPORT std::shared_ptr<PropertyValue> langOptional(const std::string& lang) const;
};

std::ostream& operator<<(std::ostream& os, const PropertyValueVector& v);

}

#endif //AUTORDF_PROPERTYVALUELIST_H