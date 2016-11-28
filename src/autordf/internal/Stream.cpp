#include "autordf/internal/Stream.h"

#include "StatementConverter.h"

namespace autordf {
namespace internal {

Stream::Stream(c_api_stream* stream) : _stream(stream) {
}

#ifdef USE_REDLAND
Stream::~Stream() {
    if (_stream) {
        librdf_free_stream(_stream);
        _stream = nullptr;
    }
}

std::shared_ptr<Statement> Stream::getObject() {
    return StatementConverter::fromCAPIStatement(librdf_stream_get_object(_stream));
}

/** Returns false if stream finished */
bool Stream::next() {
    return _stream ? !librdf_stream_next(_stream) : false;
}

/** Returns true if at end */
bool Stream::end() {
    return _stream ? librdf_stream_end(_stream) : true;
}

#elif USE_SORD
Stream::~Stream() {
    if (_stream) {
        sord_iter_free(_stream);
        _stream = nullptr;
    }
}

std::shared_ptr<Statement> Stream::getObject() {
    c_api_statement cstmt;
    if (_stream) {
        sord_iter_get(_stream, cstmt);
        return StatementConverter::fromCAPIStatement(&cstmt);
    } else {
        return nullptr;
    }
}

/** Returns false if no more objects */
bool Stream::next() {
    return _stream ? !sord_iter_next(_stream) : false;
}

/** Returns true if at end */
bool Stream::end() {
    return _stream ? sord_iter_end(_stream) : true;
}
#endif

}
}
