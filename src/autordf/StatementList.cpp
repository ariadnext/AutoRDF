#include "autordf/StatementList.h"

#include <stdexcept>

#include "autordf/Stream.h"
#include "autordf/Model.h"
#include "autordf/ModelPrivate.h"
#include "autordf/StatementConverter.h"
#include "autordf/Exception.h"

namespace autordf {

StatementListIterator::StatementListIterator(std::shared_ptr<Stream> stream) : _stream(stream)  {
    if ( _stream )  {
        _current = _stream->getObject();
    }
}

// Pre increment
StatementListIterator StatementListIterator::operator++() {
    if ( _stream->next() )  {
        _current = _stream->getObject();
    } else {
        _current.reset();
    }
    return *this;
}

bool StatementListIterator::operator==(const self_type& rhs) {
    // First handle case of comparison to end(no stream) iterator
    if ( (_stream && _stream->end() && !rhs._stream) || (rhs._stream && rhs._stream->end() && !_stream) ) {
        return true;
    } else if ( !_stream || !rhs._stream ) {
        return false;
    } else {
        throw InternalError("StatementList iterator comparison is only valid when one of the operands is end()");
    }
}

StatementList::iterator StatementList::_END(0);
StatementList::const_iterator StatementList::_CEND(0);

StatementList::iterator StatementList::begin() {
    return StatementListIterator(createNewStream());
}

StatementList::const_iterator StatementList::begin() const {
    return StatementList::const_iterator(createNewStream());
}

std::shared_ptr<Stream> StatementList::createNewStream() const {
    std::shared_ptr<librdf_statement> search(StatementConverter::toLibRdfStatement(_query));
    std::shared_ptr<Stream> stream(new Stream(librdf_model_find_statements(_m->_model->get(), search.get())));
    if ( !stream ) {
        throw InternalError("Redland librdf_model_find_statements failed");
    }
    return stream;
}
}