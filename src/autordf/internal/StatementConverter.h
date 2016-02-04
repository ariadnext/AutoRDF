#ifndef AUTORDF_STATEMENTCONVERTER_H
#define AUTORDF_STATEMENTCONVERTER_H

#include <librdf.h>

#include <memory>

#include "autordf/Statement.h"

namespace autordf {
namespace internal {

class StatementConverter {
public:
    static std::shared_ptr<librdf_statement> toLibRdfStatement(const Statement& ours);

    static std::shared_ptr<Statement> fromLibRdfStatement(librdf_statement* librdf);
};

}
}

#endif //AUTORDF_STATEMENTCONVERTER_H
