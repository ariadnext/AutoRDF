#include <autordf/Statement.h>

namespace autordf {

std::ostream& operator<<(std::ostream& os, const Statement& s) {
    os << "S: "<< s.subject << std::endl;
    os << "P: "<< s.predicate << std::endl;
    os << "O: "<< s.object << std::endl;
    return os;
}

}