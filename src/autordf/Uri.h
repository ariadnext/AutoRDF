#ifndef AUTORDF_URI_H
#define AUTORDF_URI_H

#include <librdf.h>

#include <string>

namespace autordf {
class Uri {
public:
    explicit Uri(const std::string& str);
    Uri(const Uri&) = delete;
    ~Uri();

    librdf_uri * get() const { return _uri; }

    const unsigned char * uc_str() const { return librdf_uri_as_string(_uri); }
private:
    librdf_uri *_uri;
};
}

#endif //AUTORDF_URI_H
