#ifndef AUTORDF_DEFAULT_NOTIFIER_H
#define AUTORDF_DEFAULT_NOTIFIER_H

#include <autordf/notification/ANotifier.h>

namespace autordf::notification {
/**
 * A default notifier that does nothing.
 */
class DefaultNotifier : public ANotifier {
public:
    void added(const Statement& stmt) override;
    void removed(const Statement& stmt) override;

private:
    void aggregationFinished() override;
};
} // !autordf::notification

#endif //AUTORDF_DEFAULT_NOTIFIER_H
