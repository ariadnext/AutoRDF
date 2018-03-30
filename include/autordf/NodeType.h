#ifndef AUTORDF_NODETYPE_H
#define AUTORDF_NODETYPE_H
#include <autordf/autordf_export.h>

namespace autordf {

enum class NodeType {
    /**
     * Resource node type
     */
    RESOURCE,
    /**
     * Literal value node type
     */
    LITERAL,
    /**
     * Blank node type
     */
    BLANK,
    /**
     * Empty is the only value that is not reflected on RDF data
     * It is used solely to merely identify node absence
     */
    EMPTY
};

/**
 * @param Node to convert as string
 * @return string representation for node t
 */
AUTORDF_EXPORT const char * nodeTypeString(NodeType t);

}

#endif // AUTORDF_NODETYPE_H
