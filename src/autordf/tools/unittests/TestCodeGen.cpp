#include <autordf/Factory.h>
#include <foaf/foaf.h>

#include <boost/filesystem.hpp>

#include <iostream>

int main() {
    autordf::Factory f;
    autordf::Object::setFactory(&f);

    f.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/../../unittests/foafExample.rdf", "http://xmlns.com/foaf/0.1/");

    // We can do that
    std::list<autordf::Object> list = autordf::Object::findByType("http://xmlns.com/foaf/0.1/Person");
    foaf::Person p1 = list.front().as<foaf::Person>();

    // Or directly
    std::list<foaf::Person> list2 = foaf::Person::find();
    foaf::Person p2 = list2.front();

    // FIXME: why is givenName Lost ?
    std::cout << "name = " << p2.name() << std::endl;
    std::cout << "  knows : ";
    for ( const autordf::Object& o : p2.knowsList() ) {
        std::cout << o.as<foaf::Person>().name() << ", ";
    }
}