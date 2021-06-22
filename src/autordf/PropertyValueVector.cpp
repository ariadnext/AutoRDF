#include <autordf/PropertyValueVector.h>

#include <ostream>

namespace autordf {

std::shared_ptr<PropertyValue> PropertyValueVector::langOptional(const std::string& lang) const {
    for (const PropertyValue& pv : *this) {
        if ( pv.lang() == lang ) {
            return std::make_shared<PropertyValue>(pv);
        }
    }
    return nullptr;
}

std::ostream& operator<<(std::ostream& os, const PropertyValueVector& vv) {
    for ( auto it = vv.begin(); it != vv.end(); ++it ) {
        if ( it != vv.begin() ) {
            os << ", ";
        }
        os << *it;
    }
    return os;
}

}
