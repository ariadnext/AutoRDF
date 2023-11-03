#ifndef AUTORDF_ANOTIFIER_H
#define AUTORDF_ANOTIFIER_H

#include <autordf/Statement.h>

namespace autordf::notification {
/**
 * An interface to implement to receive notification when the database is modified.
 */
class ANotifier {
public:
    virtual ~ANotifier() = default;

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

    /** Enter aggregation mode. */
    void startAggregation();
    /** Stop aggregation mode and call aggregationFinished(). */
    void releaseAggregation();
    /** Returns true or false depending if the Notifier is in aggregation mode or not. */
    bool isAggregating() const;

protected:
    /** Called by releaseAggregation. */
    virtual void aggregationFinished() = 0;

private:
    bool _aggregating = false;
};
} // !autordf::notification

#endif //AUTORDF_ANOTIFIER_H
