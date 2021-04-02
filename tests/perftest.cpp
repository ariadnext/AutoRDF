#include <locale>

#include <autordf/Factory.h>
#include <autordf/Storage.h>
#include <autordf/Object.h>

using namespace autordf;

int main(int, char **) {
    Factory f;
    Object::setFactory(&f);

    static unsigned int OBJECTS_COUNT = 3000;

    for ( unsigned int i = 0; i < OBJECTS_COUNT; i++) {
        std::stringstream ss;
        ss << "http://myobject" << i;
        Object o(ss.str());
        for ( unsigned int j = 0; j < 10; ++j ) {
            std::stringstream p;
            p << "http://property" << j;
            std::stringstream v;
            v << "http://value" << j;
            o.setPropertyValue(p.str(), PropertyValue(v.str(),""));
        }
    }

    for ( unsigned int i = 0; i < OBJECTS_COUNT; i++) {
        std::stringstream ss;
        ss << "http://myobject" << i;
        Object o(ss.str());
        o.remove();
    }
}
