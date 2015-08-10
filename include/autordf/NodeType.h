#ifndef AUTORDF_NODETYPE_H
#define AUTORDF_NODETYPE_H

namespace autordf {

enum class NodeType {
    RESOURCE,
    LITERAL,
    BLANK,
    EMPTY // Empty is the only value that is not reflected on RDF data
    // It is used solely to merely identify node absence
};

const char * nodeTypeString(NodeType t);

}

#endif // AUTORDF_NODETYPE_H
