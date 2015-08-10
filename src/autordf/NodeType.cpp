#include <autordf/NodeType.h>

#include <stdexcept>

namespace autordf {

const char * nodeTypeString(NodeType t) {
    switch(t) {
        case NodeType::RESOURCE:
            return "RESOURCE";
        case NodeType::LITERAL:
            return "LITERAL";
        case NodeType::BLANK:
            return "BLANK";
        case NodeType::EMPTY:
            return "EMPTY";
    }
}

}