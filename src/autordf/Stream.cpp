#include "autordf/Stream.h"

#include "autordf/StatementConverter.h"

namespace autordf {

Stream::Stream(librdf_stream * stream) : _stream(stream) {
}

Stream::~Stream() {
    if (_stream) {
        librdf_free_stream(_stream);
        _stream = 0;
    }
}

std::shared_ptr<Statement> Stream::getObject() {
    return StatementConverter::fromLibRdfStatement(librdf_stream_get_object(_stream));
}
}