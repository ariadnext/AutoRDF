#include "autordf/NodeList.h"

#include <stdexcept>

#include "autordf/internal/Iterator.h"
#include "autordf/Model.h"
#include "autordf/internal/ModelPrivate.h"
#include "autordf/Exception.h"

namespace autordf {

using namespace internal;

NodeIteratorBase::NodeIteratorBase(std::shared_ptr<Iterator> iterator) : _iterator(iterator)  {
    if ( _iterator && _iterator->object() )  {
        _current = std::make_shared<Node>(static_cast<librdf_node*>(_iterator->object()), false);
    }
}

void NodeIteratorBase::operatorPlusPlusHelper() {
    if ( _iterator->next() )  {
        _current = std::make_shared<Node>(static_cast<librdf_node*>(_iterator->object()), false);
    } else {
        _current.reset();
    }
}

bool NodeIteratorBase::operatorEqualsHelper(const std::shared_ptr<Iterator>& rhs) const {
    // First handle case of comparison to end(no Iterator) iterator
    if ( (_iterator && _iterator->end() && !rhs) || (rhs && rhs->end() && !_iterator) ) {
        return true;
    } else if ( !_iterator || !rhs ) {
        return false;
    } else {
        throw InternalError("NodeList iterator comparison is only valid when one of the operands is end()");
    }
}

NodeList::iterator NodeList::_END(0);
NodeList::const_iterator NodeList::_CEND(0);

NodeList::NodeList(const Node& s, const Node& p, const Node& o, const Model *m) : _subject(s), _predicate(p), _object(o), _m(m) {
    unsigned int emptyCount = 0;
    if ( _subject.empty() ) emptyCount++;
    if ( _predicate.empty() ) emptyCount++;
    if ( _object.empty() ) emptyCount++;
    if ( emptyCount !=  1 ) {
        throw InternalError("Exactly one node must be empty for NodeList constructor");
    }
}


NodeList::iterator NodeList::begin() {
    return NodeListIterator(createNewIterator());
}

NodeList::const_iterator NodeList::begin() const {
    return NodeList::const_iterator(createNewIterator());
}

std::shared_ptr<Iterator> NodeList::createNewIterator() const {
    std::shared_ptr<Iterator> it;

    if ( _subject.empty() ) {
        it = std::make_shared<Iterator>(librdf_model_get_sources(_m->_model->get(), _predicate.get(), _object.get()));
    }
    if ( _predicate.empty() ) {
        it = std::make_shared<Iterator>(librdf_model_get_arcs(_m->_model->get(), _subject.get(), _object.get()));
    }
    if ( _object.empty() ) {
        it = std::make_shared<Iterator>(librdf_model_get_targets(_m->_model->get(), _subject.get(), _predicate.get()));
    }
    return it;
}

std::ostream& operator<<(std::ostream& os, const NodeList& nl) {
    for ( auto const& node : nl) {
        os << node << std::endl;
    }
    return os;
}

}