#ifndef AUTORDF_STATEMENTLIST_H
#define AUTORDF_STATEMENTLIST_H

#include <iterator>
#include <memory>

#include <autordf/Statement.h>

namespace autordf {

class Stream;
class Statement;

class StatementListIterator: public std::iterator<std::forward_iterator_tag, Statement> {
    std::shared_ptr<Stream> _stream;
    std::shared_ptr<Statement> _current;
public:
    typedef StatementListIterator self_type;

    StatementListIterator(std::shared_ptr<Stream> stream);

    // Only pre increment is supported in this iterator;
    self_type operator++();

    reference operator*() { return *_current.get(); }
    pointer operator->() { return _current.get(); }
    bool operator==(const self_type& rhs);
    bool operator!=(const self_type& rhs) { return !operator==(rhs); }
};

/**
 * Only supposed to be constructed by model class
 */
class StatementList {
public:
    typedef StatementListIterator iterator;

    StatementList(std::shared_ptr<Stream> stream);

    StatementListIterator begin() { return _begin; }
    StatementListIterator end() { return _end; }

private:
    iterator _begin;
    iterator _end;
};

}

#endif //AUTORDF_STATEMENTLIST_H
