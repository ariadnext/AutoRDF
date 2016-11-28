#ifndef AUTORDF_STATEMENTCONVERTER_H
#define AUTORDF_STATEMENTCONVERTER_H

#include <memory>

#include <autordf/internal/cAPI.h>

#include <autordf/Statement.h>

namespace autordf {
namespace internal {

class StatementConverter {
public:
    /**
     * Once called ours is emptyed
     */
#if defined(USE_REDLAND)
    static std::shared_ptr<c_api_statement> toCAPIStatement(Statement *ours);
#elif defined(USE_SORD)
    static void toCAPIStatement(const Statement *ours, SordQuad *cstmt);
#endif

    static std::shared_ptr<Statement> fromCAPIStatement(c_api_statement* librdf);
};

}
}

#endif //AUTORDF_STATEMENTCONVERTER_H
