#ifndef AUTORDF_STATEMENTLIST_H
#define AUTORDF_STATEMENTLIST_H

#include <iterator>
#include <memory>

#include <autordf/Statement.h>

namespace autordf {

class Stream;
class Statement;
class Model;

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

class StatementListConstIterator: public std::iterator<std::forward_iterator_tag, const Statement> {
    std::shared_ptr<Stream> _stream;
    std::shared_ptr<Statement> _current;
public:
    typedef StatementListConstIterator self_type;

    StatementListConstIterator(std::shared_ptr<Stream> stream);

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
    typedef StatementListIterator const_iterator;

    StatementList(const Statement& query, Model *m) : _query(query), _m(m) {}

    iterator begin();
    iterator end() { return _END; }

    const_iterator begin() const;
    const_iterator end() const { return _CEND; }

    size_t size() const { return std::distance(begin(), end()); }

private:
    static iterator _END;
    static const_iterator _CEND;
    Statement _query;
    Model    *_m;

    std::shared_ptr<Stream> createNewStream() const;
};

}

#endif //AUTORDF_STATEMENTLIST_H
