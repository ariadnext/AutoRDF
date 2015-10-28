#ifndef AUTORDF_STREAM_H
#define AUTORDF_STREAM_H

#include <librdf.h>

#include <memory>

namespace autordf {
class Statement;

namespace internal {

class Stream {
public:
    Stream(librdf_stream* stream);

    ~Stream();

    /** Returns the statement we currently point to */
    std::shared_ptr<Statement> getObject();

    /** Returns false if stream finished */
    bool next() { return _stream ? !librdf_stream_next(_stream) : false; }

    /** Returns true if at end */
    bool end() { return _stream ? librdf_stream_end(_stream) : true; }

    librdf_stream* get() const { return _stream; }

private:
    librdf_stream* _stream;
};

}
}

#endif //AUTORDF_STREAM_H
