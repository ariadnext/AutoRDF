#ifndef AUTORDF_INOTIFIER_H
#define AUTORDF_INOTIFIER_H

#include <autordf/Statement.h>

namespace autordf {
/**
 * An interface to implement to receive notification when the database is modified.
 */
class INotifier {
public:
    /**
     * Called when a statement is added to the database
     *
     * @param stmt the newly added statement
     */
    virtual void added(const Statement& stmt) = 0;

    /**
     * Called when a statement is removed from the database
     *
     * @param stmt the just removed statement
     */
    virtual void removed(const Statement& stmt) = 0;
};
} // !autordf

#endif //AUTORDF_INOTIFIER_H
