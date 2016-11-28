#include "StatementConverter.h"

#include <stdexcept>

#include "autordf/internal/World.h"
#include "autordf/Exception.h"

namespace autordf {
namespace internal {

#if defined(USE_REDLAND)
std::shared_ptr<c_api_statement> StatementConverter::toCAPIStatement(Statement *stmt) {
    World w;
    std::shared_ptr<c_api_statement> cstmt(librdf_new_statement(w.get()), librdf_free_statement);
    if (!stmt->subject.empty()) {
        librdf_statement_set_subject(cstmt.get(), stmt->subject.pull());
    }
    if (!stmt->predicate.empty()) {
        librdf_statement_set_predicate(cstmt.get(), stmt->predicate.pull());
    }
    if (!stmt->object.empty()) {
        librdf_statement_set_object(cstmt.get(), stmt->object.pull());
    }
    return cstmt;
}

std::shared_ptr<Statement> StatementConverter::fromCAPIStatement(c_api_statement *cstmt) {
    auto statement = std::make_shared<Statement>();
    if (cstmt) {
        return std::make_shared<Statement>(
                Node(librdf_statement_get_subject(cstmt), false),
                Node(librdf_statement_get_predicate(cstmt), false),
                Node(librdf_statement_get_object(cstmt), false));
    } else {
        return std::make_shared<Statement>();
    }
    return statement;
}

#elif(USE_SORD)

void StatementConverter::toCAPIStatement(const Statement *stmt, SordQuad *cstmt) {
    for ( unsigned int i = 0; i <= SordQuadIndex::SORD_GRAPH; ++i ) {
        (*cstmt)[i] = nullptr;
    }
    if (!stmt->subject.empty()) {
        (*cstmt)[SordQuadIndex::SORD_SUBJECT] =  stmt->subject.get();
    }
    if (!stmt->predicate.empty()) {
        (*cstmt)[SordQuadIndex::SORD_PREDICATE] = stmt->predicate.get();
    }
    if (!stmt->object.empty()) {
        (*cstmt)[SordQuadIndex::SORD_OBJECT] = stmt->object.get();
    }
}

std::shared_ptr<Statement> StatementConverter::fromCAPIStatement(SordQuad *cstmt) {
    auto statement = std::make_shared<Statement>();
    if (cstmt) {
        return std::make_shared<Statement>(
                // Node copy constructor is called in order to create a Node copy that won't be won by sord
                Node(Node(const_cast<SordNode*>((*cstmt)[SordQuadIndex::SORD_SUBJECT]), false)),
                Node(Node(const_cast<SordNode*>((*cstmt)[SordQuadIndex::SORD_PREDICATE]), false)),
                Node(Node(const_cast<SordNode*>((*cstmt)[SordQuadIndex::SORD_OBJECT]), false)));
    } else {
        return std::make_shared<Statement>();
    }
    return statement;
}

#endif

}
}