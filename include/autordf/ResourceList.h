#ifndef AUTORDF_RESOURCELIST_H
#define AUTORDF_RESOURCELIST_H

#include <iterator>
#include <memory>

#include <autordf/Resource.h>
#include <autordf/StatementList.h>

namespace autordf {

class Factory;

class ResourceListIterator: public std::iterator<std::forward_iterator_tag, Resource> {
    StatementListIterator _stmtit;
    std::shared_ptr<Resource> _current;
    bool _inSync;
    Factory *_f;

    void sync();
public:
    typedef ResourceListIterator self_type;

    ResourceListIterator(StatementListIterator stmtit, Factory *f) : _stmtit(stmtit), _inSync(false), _f(f) {}

    // Only pre increment is supported in this iterator;
    self_type operator++() { _inSync = false; ++_stmtit; return *this; }

    reference operator*() { sync(); return *_current; }
    pointer operator->() { sync(); return _current.get(); }
    bool operator==(const self_type& rhs) { return _stmtit == rhs._stmtit; }
    bool operator!=(const self_type& rhs) { return !operator==(rhs); }
};


/**
 * Only supposed to be constructed by model class
 */
class ResourceList {
public:
    typedef ResourceListIterator iterator;
    typedef ResourceListIterator const_iterator;
    typedef unsigned int size_t;

    ResourceList(const Statement& query, Factory *f) : _query(query), _f(f) {}

    iterator begin();
    iterator end() { return _END; }

    const_iterator begin() const;
    const_iterator end() const { return _CEND; }

    size_t size() const { return std::distance(begin(), end()); }

private:
    static iterator _END;
    static const_iterator _CEND;
    Statement _query;
    Factory    *_f;
};

}

#endif //AUTORDF_RESOURCELIST_H
