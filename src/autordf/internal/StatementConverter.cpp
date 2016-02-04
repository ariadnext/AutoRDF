#include "StatementConverter.h"

#include <stdexcept>

#include "autordf/internal/World.h"
#include "autordf/internal/NodeConverter.h"
#include "autordf/Exception.h"

namespace autordf {
namespace internal {

std::shared_ptr<librdf_statement> StatementConverter::toLibRdfStatement(const Statement& stmt) {
    World w;
    std::shared_ptr<librdf_statement> lrdfstatement(librdf_new_statement(w.get()), librdf_free_statement);
    if (!stmt.subject.empty()) {
        librdf_statement_set_subject(lrdfstatement.get(), NodeConverter::toLibRdfNode(stmt.subject));
    }
    if (!stmt.predicate.empty()) {
        librdf_statement_set_predicate(lrdfstatement.get(), NodeConverter::toLibRdfNode(stmt.predicate));
    }
    if (!stmt.object.empty()) {
        librdf_statement_set_object(lrdfstatement.get(), NodeConverter::toLibRdfNode(stmt.object));
    }
    return lrdfstatement;
}

std::shared_ptr<Statement> StatementConverter::fromLibRdfStatement(librdf_statement* lrdfstatement) {
    auto statement = std::make_shared<Statement>();
    if (lrdfstatement) {
        NodeConverter::fromLibRdfNode(librdf_statement_get_subject(lrdfstatement), &statement->subject);
        NodeConverter::fromLibRdfNode(librdf_statement_get_predicate(lrdfstatement), &statement->predicate);
        NodeConverter::fromLibRdfNode(librdf_statement_get_object(lrdfstatement), &statement->object);
    }
    return statement;
}

}
}