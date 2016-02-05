#include <autordf/PropertyValueVector.h>

namespace autordf {

std::shared_ptr<PropertyValue> PropertyValueVector::langOptional(const std::string& lang) const {
    for (const PropertyValue& pv : *this) {
        if ( pv.lang() == lang ) {
            return std::make_shared<PropertyValue>(pv);
        }
    }
    return nullptr;
}

}
