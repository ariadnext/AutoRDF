#ifndef AUTORDF_STREAM_H
#define AUTORDF_STREAM_H

#include <memory>
#include <autordf/internal/cAPI.h>

namespace autordf {
class Statement;

namespace internal {

class Stream {
public:
    Stream(c_api_stream* stream);

    ~Stream();

    /** Returns the statement we currently point to */
    std::shared_ptr<Statement> getObject();

    /** Returns false if stream finished */
    bool next();

    /** Returns true if at end */
    bool end();

    c_api_stream* get() const { return _stream; }

private:
    c_api_stream* _stream;
};

}
}

#endif //AUTORDF_STREAM_H
