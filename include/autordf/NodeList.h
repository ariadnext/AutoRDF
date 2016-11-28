#ifndef AUTORDF_NODELIST_H
#define AUTORDF_NODELIST_H

#include <iterator>
#include <memory>
#include <iosfwd>
#include <vector>

#include <autordf/Node.h>

namespace autordf {

class Model;
class Node;

namespace internal {
class Iterator;
}

#if defined(USE_REDLAND)

//! @cond Doxygen_Suppress
class NodeIteratorBase {
protected:
    std::shared_ptr<internal::Iterator>    _iterator;
    std::shared_ptr<Node> _current;
    NodeIteratorBase(std::shared_ptr<internal::Iterator> iterator);

    void operatorPlusPlusHelper();
    bool operatorEqualsHelper(const std::shared_ptr<internal::Iterator>& rhs) const;
};

template<typename T>
class NodeListIterator_: public NodeIteratorBase, public std::iterator<std::forward_iterator_tag, Node> {
public:
    typedef NodeListIterator_ self_type;

    NodeListIterator_(std::shared_ptr<internal::Iterator> iterator) : NodeIteratorBase(iterator) {};

    // Only pre increment is supported in this iterator;
    self_type operator++() {operatorPlusPlusHelper(); return *this;}

    reference operator*() { return *_current.get(); }
    pointer operator->() { return _current.get(); }
    bool operator==(const self_type& rhs) const { return operatorEqualsHelper(rhs._iterator); }
    bool operator!=(const self_type& rhs) { return !operator==(rhs); }
};

typedef NodeListIterator_<Node> NodeListIterator;
typedef NodeListIterator_<const Node> NodeListConstIterator;

/**
 * Only supposed to be constructed by model class
 */
class NodeList {
public:
    typedef NodeListIterator iterator;
    typedef NodeListConstIterator const_iterator;

    iterator begin();
    iterator end() { return _END; }

    const_iterator begin() const;
    const_iterator end() const { return _CEND; }

    size_t size() const { return std::distance(begin(), end()); }

private:
    static iterator _END;
    static const_iterator _CEND;

#elif defined(USE_SORD)
class NodeList : public std::vector<Node> {
private:
#endif
    enum class Mode {
        ARCSIN,
        ARCSOUT,
        DEFAULT
    };
    Mode _mode;
    // in default mode one of those three must be empty
    // in arcs mode, both predicate and object are empty
    Node      _subject;
    Node      _predicate;
    Node      _object;
    const Model    *_m;

    std::shared_ptr<internal::Iterator> createNewIterator() const;

    NodeList(const Node& s, Mode arcsMode, const Model *m);

    NodeList(const Node& s, const Node& p, const Node& o, const Model *m);

    void consistencyCheck();

    friend class Model;
};

std::ostream& operator<<(std::ostream& os, const NodeList& s);

}

#endif //AUTORDF_NODELIST_H
