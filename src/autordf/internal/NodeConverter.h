#ifndef AUTORDF_NODECONVERTER_H
#define AUTORDF_NODECONVERTER_H

#include <librdf.h>

#include <memory>

#include "autordf/Node.h"

namespace autordf {
namespace internal {

class NodeConverter {
public:
    static librdf_node* toLibRdfNode(const Node& node);;
    static std::shared_ptr<librdf_node> toLibRdfNodeSmartPtr(const Node& node);;

    static void fromLibRdfNode(librdf_node* librdf, Node* ours);
};

}
}

#endif //AUTORDF_NODECONVERTER_H
