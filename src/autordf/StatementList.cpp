#include "autordf/StatementList.h"

#include "autordf/Stream.h"

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
    } else {
        return _stream == rhs._stream;
    }
}

StatementList::StatementList(std::shared_ptr<Stream> stream): _begin(stream), _end(0) {
}

}