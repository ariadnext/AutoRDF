#include "autordf/internal/StatementConverter.h"

#include <stdexcept>

#include "autordf/internal/World.h"
#include "autordf/Exception.h"

namespace autordf {
namespace internal {

librdf_node* StatementConverter::toLibRdfNode(const Node& node) {
    librdf_node * lrdfNode = 0;
    World w;
    switch (node.type) {
        case NodeType::RESOURCE: {
            std::shared_ptr<librdf_uri> uri(
                    librdf_new_uri(w.get(), reinterpret_cast<const unsigned char*>(node.iri().c_str())),
                    librdf_free_uri);
            if (!uri) {
                throw InternalError(std::string("Failed to construct URI from value: ") + node.iri());
            }
            lrdfNode = librdf_new_node_from_uri(w.get(), uri.get());
            if (!lrdfNode) {
                throw InternalError("Failed to construct node from URI");
            }
            break;
        }
        case NodeType::LITERAL: {
            std::shared_ptr<librdf_uri> dataTypeUri;
            if ( !node.dataType().empty() ) {
                dataTypeUri = std::shared_ptr<librdf_uri>(librdf_new_uri(w.get(), reinterpret_cast<const unsigned char*>(node.dataType().c_str())),
                                librdf_free_uri);
                if (!dataTypeUri) {
                    throw InternalError(std::string("Failed to construct URI from value: ") + node.dataType());
                }
            }
            lrdfNode = librdf_new_node_from_typed_literal(w.get(),
                                                    reinterpret_cast<const unsigned char*>(node.literal().c_str()), node.lang().c_str(),
                                                    dataTypeUri.get());
            if (!lrdfNode) {
                throw InternalError(std::string("Failed to construct node from literal: ") + node.literal());
            }
            break;
        }
        case NodeType::BLANK:
            lrdfNode = librdf_new_node_from_blank_identifier(w.get(),
                                                             reinterpret_cast<const unsigned char*>(node.bNodeId().c_str()));
            if (!lrdfNode) {
                throw InternalError(std::string("Failed to construct node from blank identifier: ") +
                                    node.bNodeId());
            }
            break;
        default:
            // Ignore other node types, as they don't exit in rdf
            ;
    }
    return lrdfNode;
}

std::shared_ptr<librdf_statement> StatementConverter::toLibRdfStatement(const Statement& stmt) {
    World w;
    std::shared_ptr<librdf_statement> lrdfstatement(librdf_new_statement(w.get()), librdf_free_statement);
    if (!stmt.subject.empty()) {
        librdf_statement_set_subject(lrdfstatement.get(), toLibRdfNode(stmt.subject));
    }
    if (!stmt.predicate.empty()) {
        librdf_statement_set_predicate(lrdfstatement.get(), toLibRdfNode(stmt.predicate));
    }
    if (!stmt.object.empty()) {
        librdf_statement_set_object(lrdfstatement.get(), toLibRdfNode(stmt.object));
    }
    return lrdfstatement;
}

void StatementConverter::fromLibRdfNode(librdf_node* lrdfnode, Node* node) {
    if (librdf_node_is_resource(lrdfnode)) {
        node->type = NodeType::RESOURCE;
        librdf_uri * uri = librdf_node_get_uri(lrdfnode);
        node->_value = reinterpret_cast<const char*>(librdf_uri_as_string(uri));
    } else if (librdf_node_is_literal(lrdfnode)) {
        node->type = NodeType::LITERAL;
        node->_value = reinterpret_cast<const char*>(librdf_node_get_literal_value(lrdfnode));
        librdf_uri *dataTypeUri = librdf_node_get_literal_value_datatype_uri(lrdfnode);
        if (dataTypeUri) {
            node->_dataType = reinterpret_cast<const char*>(librdf_uri_as_string(dataTypeUri));
        }
        if (librdf_node_get_literal_value_language(lrdfnode)) {
            node->_lang = librdf_node_get_literal_value_language(lrdfnode);
        }
    } else if (librdf_node_is_blank(lrdfnode)) {
        node->type = NodeType::BLANK;
        node->_value = reinterpret_cast<const char*>(librdf_node_get_blank_identifier(lrdfnode));
    } else {
        throw InternalError("librdf_node_get_type: unknown node type encountered");
    }
}

std::shared_ptr<Statement> StatementConverter::fromLibRdfStatement(librdf_statement* lrdfstatement) {
    auto statement = std::make_shared<Statement>();
    if (lrdfstatement) {
        fromLibRdfNode(librdf_statement_get_subject(lrdfstatement), &statement->subject);
        fromLibRdfNode(librdf_statement_get_predicate(lrdfstatement), &statement->predicate);
        fromLibRdfNode(librdf_statement_get_object(lrdfstatement), &statement->object);
    }
    return statement;
}

}
}