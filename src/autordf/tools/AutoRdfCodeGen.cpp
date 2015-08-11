#include <iostream>

#include <autordf/Object.h>
#include <autordf/Factory.h>

namespace autordf {
namespace tools {

static const std::string RDFS = "http://www.w3.org/2000/01/rdf-schema";

void run() {
    std::list<Object> rdfsClasses = Object::findByType(RDFS + "#Class");
    for ( auto o : rdfsClasses) {
        std::cout << "------------------------------------------" << std::endl;
        std::cout << o << std::endl;
    }
}

}
}


int main(int argc, char **argv) {
    autordf::Factory f;
    autordf::Object::setFactory(&f);
    f.loadFromFile(argv[1], "http://xmlns.com/foaf/0.1/Person");
    autordf::tools::run();
    return 0;
}