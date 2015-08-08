#ifndef AUTORDF_STATEMENTCONVERTER_H
#define AUTORDF_STATEMENTCONVERTER_H

#include <librdf.h>

#include <memory>

#include "autordf/Node.h"
#include "autordf/Statement.h"

namespace autordf {
class StatementConverter {
public:
    static std::shared_ptr<librdf_statement> toLibRdf(const Statement& ours);
    static std::shared_ptr<Statement> fromLibRdf(librdf_statement* librdf);

private:
    static void toLibRdf(const Node& ours, librdf_node *librdf);
    static void fromLibRdf(librdf_node *librdf, Node *ours);
};
}

#endif //AUTORDF_STATEMENTCONVERTER_H
