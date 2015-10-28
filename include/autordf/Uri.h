//
// Created by sbaguet on 10/28/15.
//

#ifndef AUTORDF_URI_H
#define AUTORDF_URI_H

#include <string>

namespace autordf {

class Uri : public std::string {
public:
    /**
     * Base class constructor inheritance
     */
    using std::string::string;
    /**
     * Constructor to make Uri from string.
     * Should be automatically done by above declaration but not working
     */
    Uri(const std::string& str) : std::string(str) {};
};

}


#endif //AUTORDF_URI_H
