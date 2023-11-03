#include <autordf/notification/ANotifier.h>

namespace autordf::notification {
void ANotifier::startAggregation() {
    _aggregating = true;
}

void ANotifier::releaseAggregation() {
    aggregationFinished();
    _aggregating = false;
}

bool ANotifier::isAggregating() const {
    return _aggregating;
}
} // !autordf::notification
