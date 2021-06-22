#include <autordf/Factory.h>
#include <foaftest/owl/owl.h>
#include <foaftest/foaf/foaf.h>

#include <boost/filesystem.hpp>

#include <iostream>

using namespace foaftest;

int main() {
    autordf::Factory f;
    autordf::Object::setFactory(&f);

    f.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/../../unittests/foafExample.ttl");

    // We can do that
    std::vector<autordf::Object> list = autordf::Object::findByType("http://xmlns.com/foaf/0.1/Person");
    foaf::Person p1 = list.front().as<foaf::Person>();

    // Or directly
    std::vector<foaf::Person> list2 = foaf::Person::find();
    foaf::Person p2 = list2.front();

    // FIXME: why is givenName Lost ?
    std::cout << "name = " << p2.nameList().front() << std::endl;
    std::cout << "  knows : ";
    for ( const foaf::Person& o : p2.knowsList() ) {
        std::cout << o.nameList().front() << ", ";
    }
}