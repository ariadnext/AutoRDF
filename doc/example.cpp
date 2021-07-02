#include <autordf/Factory.h>

#include <geometry/geo/geo.h>

using namespace geometry;

int main(int, char **) {
    autordf::Factory f;
    f.addNamespacePrefix("geo", "http://example.org/geometry#");
    autordf::Object::setFactory(&f);

    geo::Rectangle r("http://example.org/myfancyrectangle");

    // Set one of my rectangle coordinates - the long way
    geo::Point tl;
    tl.setX(1.0);
    tl.setY(2.0);
    r.setTopLeft(tl);

    // Set one of my rectangle coordinates - the short way
    r.setBottomRight(geo::Point().setX(11).setY(12));

    // Set result of my work
    f.saveToFile(stdout, "turtle");
}
