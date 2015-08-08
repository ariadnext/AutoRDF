#ifndef AUTORDF_STATEMENT_H
#define AUTORDF_STATEMENT_H

#include <autordf/Node.h>

namespace autordf {

class Statement {
public:
    Node subject;
    Node predicate;
    Node object;
};

}

#endif //AUTORDF_STATEMENT_H
