#ifndef AUTORDF_STATEMENT_H
#define AUTORDF_STATEMENT_H

#include <iosfwd>

#include <autordf/Node.h>

namespace autordf {

/**
 * A Statement is uses both:
 *  - As a container of data retrieved from the model
 *  - As a query language to find statement in the model: subject, predicate, object can be empty or partially filled
 */
class Statement {
public:
    /**
     * Statement subject
     *
     * Can be empty (empty value or EMPTY type) when used as a model query
     */
    Node subject;

    /**
     * Statement predicate
     *
     * Can be empty (empty value or EMPTY type) when used as a model query
     */
    Node predicate;

    /**
     * Statement object
     *
     * Can be empty (empty value or EMPTY type) when used as a model query
     */
    Node object;
};

/**
 * Dumps a statement to stream.
 *
 * Format is
 * S: {Subject}
 * P: {Predicate}
 * O: {Object}
 */
std::ostream& operator<<(std::ostream& os, const Statement&);
}

#endif //AUTORDF_STATEMENT_H
