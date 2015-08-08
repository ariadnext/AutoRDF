#include <stdexcept>
#include "StatementConverter.h"

namespace autordf {

//static std::shared_ptr<librdf_statement> StatementConverter::toLibRdf(const Statement& ours)
//}

void StatementConverter::fromLibRdf(librdf_node *lrdfnode, Node *node) {
    if (librdf_node_is_resource(lrdfnode)) {
        node->type = Node::RESOURCE;
        librdf_uri *uri = librdf_node_get_uri(lrdfnode);
        node->_value = (const char *)librdf_uri_as_string(uri);
    } else if (librdf_node_is_literal(lrdfnode)) {
        node->type = Node::LITERAL;
        node->_value = (const char *)librdf_node_get_literal_value(lrdfnode);
    } else if (librdf_node_is_blank(lrdfnode)) {
        node->type = Node::BLANK;
        node->_value = (const char *)librdf_node_get_blank_identifier(lrdfnode);
    } else {
        throw std::runtime_error("librdf_node_get_type: unknown node type encountered");
    }
}

std::shared_ptr<Statement> StatementConverter::fromLibRdf(librdf_statement* lrdfstatement) {
    auto statement = std::make_shared<Statement>();
    if ( lrdfstatement ) {
        fromLibRdf(librdf_statement_get_subject(lrdfstatement), &statement->subject);
        fromLibRdf(librdf_statement_get_predicate(lrdfstatement), &statement->predicate);
        fromLibRdf(librdf_statement_get_object(lrdfstatement), &statement->object);
    }
    return statement;
}

}