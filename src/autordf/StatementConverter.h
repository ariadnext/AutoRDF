#ifndef AUTORDF_STATEMENTCONVERTER_H
#define AUTORDF_STATEMENTCONVERTER_H

#include <librdf.h>

#include <memory>

#include "autordf/Node.h"
#include "autordf/Statement.h"

namespace autordf {
class StatementConverter {
public:
    static std::shared_ptr<librdf_statement> toLibRdfStatement(const Statement &ours);
    static std::shared_ptr<Statement> fromLibRdfStatement(librdf_statement *librdf);

private:
    static librdf_node*toLibRdfNode(const Node &node);;
    static void fromLibRdfNode(librdf_node *librdf, Node *ours);
};
}

#endif //AUTORDF_STATEMENTCONVERTER_H
