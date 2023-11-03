#include <autordf/notification/NotifierLocker.h>

namespace autordf::notification {
NotifierLocker::NotifierLocker(ANotifier &notifier) : _notifier(notifier) {
    if (!_notifier.isAggregating()) {
        _notifier.startAggregation();
        _triggeredAggregation = true;
    }
}

NotifierLocker::~NotifierLocker() {
    if (_triggeredAggregation) {
        _notifier.releaseAggregation();
    }
}

bool NotifierLocker::triggeredAggregation() const {
    return _triggeredAggregation;
}

ANotifier const &NotifierLocker::notifier() const {
    return _notifier;
}
} // !autordf::notification
