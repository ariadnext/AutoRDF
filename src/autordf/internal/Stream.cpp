#include "autordf/internal/Stream.h"

#include "StatementConverter.h"

namespace autordf {
namespace internal {

Stream::Stream(librdf_stream* stream) : _stream(stream) {
}

Stream::~Stream() {
    if (_stream) {
        librdf_free_stream(_stream);
        _stream = nullptr;
    }
}

std::shared_ptr<Statement> Stream::getObject() {
    return StatementConverter::fromLibRdfStatement(librdf_stream_get_object(_stream));
}

}
}