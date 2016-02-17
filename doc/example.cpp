#include <autordf/Factory.h>

#include <geometry/geo/geo.h>

using namespace geometry;

int main(int argc, char ** argv) {
    autordf::Factory f;
    autordf::Object::setFactory(&f);

    geo::Rectangle r;
    geo::Point tl;
    tl.setX(1.0);
    tl.setY(2.0);

    r.setTopLeft(tl);
    r.setBottomRight(geo::Point().setX(11).setY(12));

    f.saveToFile("/tmp/file.ttl");
}
