#ifndef AUTORDF_STATEMENT_H
#define AUTORDF_STATEMENT_H

#include <iosfwd>

#include <autordf/Node.h>

namespace autordf {

class Statement {
public:
    Node subject;
    Node predicate;
    Node object;
};

std::ostream& operator<<(std::ostream& os, const Statement&);
}

#endif //AUTORDF_STATEMENT_H
