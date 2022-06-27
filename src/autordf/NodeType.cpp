#include <autordf/NodeType.h>

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
    // This enables compiling with GCC (function should return a value).
    // Clang was fine as it detected that all enum types were handled.
    // If casting NodeType with a random integer, Clang returns "EMPTY" (last enum value).
    // With this return, GCC and Clang produce the same behaviour.
    return "EMPTY";
}

}