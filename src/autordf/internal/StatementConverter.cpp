#include "StatementConverter.h"

#include <stdexcept>

#include "autordf/internal/World.h"
#include "autordf/Exception.h"

namespace autordf {
namespace internal {

std::shared_ptr<librdf_statement> StatementConverter::toLibRdfStatement(const Statement& stmt) {
    World w;
    std::shared_ptr<librdf_statement> lrdfstatement(librdf_new_statement(w.get()), librdf_free_statement);
    if (!stmt.subject.empty()) {
        librdf_statement_set_subject(lrdfstatement.get(), stmt.subject.get());
    }
    if (!stmt.predicate.empty()) {
        librdf_statement_set_predicate(lrdfstatement.get(), stmt.predicate.get());
    }
    if (!stmt.object.empty()) {
        librdf_statement_set_object(lrdfstatement.get(), stmt.object.get());
    }
    return lrdfstatement;
}

std::shared_ptr<Statement> StatementConverter::fromLibRdfStatement(librdf_statement* lrdfstatement) {
    auto statement = std::make_shared<Statement>();
    if (lrdfstatement) {
        return std::make_shared<Statement>(
                Node(librdf_statement_get_subject(lrdfstatement), false),
                Node(librdf_statement_get_predicate(lrdfstatement), false),
                Node(librdf_statement_get_object(lrdfstatement), false));
    } else {
        return std::make_shared<Statement>();
    }
    return statement;
}

}
}