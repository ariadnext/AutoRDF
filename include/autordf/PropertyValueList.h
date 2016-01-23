#ifndef AUTORDF_PROPERTYVALUELIST_H
#define AUTORDF_PROPERTYVALUELIST_H

#include <list>
#include <memory>

#include <autordf/PropertyValue.h>

namespace autordf {

/**
 * Stores a List of values(aka Literal) of a Web Semantic Resource
 */
class PropertyValueList : public std::list<PropertyValue> {
public:
    using std::list<PropertyValue>::list;

    /**
     * Returns the Value from the whose language is idendified by lang
     * If not found, returns nullptr
     */
    std::shared_ptr<PropertyValue> langOptional(const std::string& lang) const;
};

}

#endif //AUTORDF_PROPERTYVALUELIST_H