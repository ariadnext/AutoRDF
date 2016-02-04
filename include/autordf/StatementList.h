#ifndef AUTORDF_STATEMENTLIST_H
#define AUTORDF_STATEMENTLIST_H

#include <iterator>
#include <memory>
#include <iosfwd>

#include <autordf/Statement.h>

namespace autordf {

class Model;
class Statement;

namespace internal {
class Stream;
}

//! @cond Doxygen_Suppress
class StatementIteratorBase {
protected:
    std::shared_ptr<internal::Stream>    _stream;
    std::shared_ptr<Statement> _current;
    StatementIteratorBase(std::shared_ptr<internal::Stream> stream);

    void operatorPlusPlusHelper();
    bool operatorEqualsHelper(const std::shared_ptr<internal::Stream>& rhs) const;
};

template<typename T>
class StatementListIterator_: public StatementIteratorBase, public std::iterator<std::forward_iterator_tag, const Statement> {
public:
    typedef StatementListIterator_ self_type;

    StatementListIterator_(std::shared_ptr<internal::Stream> stream) : StatementIteratorBase(stream) {};

    // Only pre increment is supported in this iterator;
    self_type operator++() {operatorPlusPlusHelper(); return *this;}

    reference operator*() { return *_current.get(); }
    pointer operator->() { return _current.get(); }
    bool operator==(const self_type& rhs) const { return operatorEqualsHelper(rhs._stream); }
    bool operator!=(const self_type& rhs) { return !operator==(rhs); }
};

typedef StatementListIterator_<Statement> StatementListIterator;
typedef StatementListIterator_<const Statement> StatementListConstIterator;

/**
 * Only supposed to be constructed by model class
 */
class StatementList {
public:
    typedef StatementListIterator iterator;
    typedef StatementListConstIterator const_iterator;

    iterator begin();
    iterator end() { return _END; }

    const_iterator begin() const;
    const_iterator end() const { return _CEND; }

    size_t size() const { return std::distance(begin(), end()); }

private:
    static iterator _END;
    static const_iterator _CEND;
    Statement _query;
    const Model    *_m;

    std::shared_ptr<internal::Stream> createNewStream() const;

    StatementList(const Statement& query, const Model *m) : _query(query), _m(m) {}

    friend class Model;
};
//! @endcond

std::ostream& operator<<(std::ostream& os, const StatementList& s);

}

#endif //AUTORDF_STATEMENTLIST_H
