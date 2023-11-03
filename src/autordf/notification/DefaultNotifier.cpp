#include <autordf/notification/DefaultNotifier.h>

namespace autordf::notification {
    void DefaultNotifier::added(const Statement&) {}
    void DefaultNotifier::removed(const Statement&) {}
    void DefaultNotifier::aggregationFinished() {}
} // !autordf::notification
