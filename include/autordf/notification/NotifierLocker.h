#ifndef AUTORDF_NOTIFIER_LOCKER_H
#define AUTORDF_NOTIFIER_LOCKER_H

#include <autordf/notification/ANotifier.h>

namespace autordf::notification {
/**
 * While this object exists, it given Notifier is stuck in aggregation mode.
 * If the Notifier was already in aggregation mode when the object was created,
 * the aggregation mode is not released when the object is destroyed.
 * Does not take ownership of the given Notifier.
 */
class NotifierLocker {
public:
    NotifierLocker() = delete;
    explicit NotifierLocker(ANotifier &notifier);
    ~NotifierLocker();

    /** Returns true if this NotifierLocker triggered the aggregation mode on it Notifier. */
    bool triggeredAggregation() const;
    /** Returns the Notifier locked. */
    ANotifier const &notifier() const;

private:
    bool        _triggeredAggregation = false;
    ANotifier   &_notifier;
};
} // !autordf::notification

#endif //AUTORDF_NOTIFIER_LOCKER_H
