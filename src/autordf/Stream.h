#ifndef AUTORDF_STREAM_H
#define AUTORDF_STREAM_H

#include <librdf.h>

#include <memory>

namespace autordf {

class Statement;

class Stream {
public:
    Stream(librdf_stream * stream);

    ~Stream();

    /** Returns the statement we currently point to */
    std::shared_ptr<Statement> getObject();

    /** Returns 0 if stream finished */
    int next() { return _stream ? librdf_stream_next(_stream) : 0; }

    /** Returns != 0 if at end */
    int end() { return _stream ? librdf_stream_end(_stream) : 1; }

    librdf_stream *get() const { return _stream; }
private:
    librdf_stream *_stream;
};
}

#endif //AUTORDF_STREAM_H
