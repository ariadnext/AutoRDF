
#include <gtest/gtest.h>

#include <boost/filesystem.hpp>

#include "autordf/Model.h"
#include "autordf/Factory.h"
#include "autordf/Resource.h"

using namespace autordf;

TEST(_02_LoadSave, OneProperty) {
    Factory f;
    f.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/foafExample.rdf");

    Resource r = f.createIRIResource("http://jimmycricket.com/me");
    ASSERT_EQ("Jimmy Criket", r.getProperty("http://xmlns.com/foaf/0.1/name").value());
    ASSERT_EQ("Jimmy Criket", r.getOptionalProperty("http://xmlns.com/foaf/0.1/name")->value());
}

TEST(_02_LoadSave, PropertyNotThere) {
    Factory f;
    f.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/foafExample.rdf");

    Resource r = f.createIRIResource("http://jimmycricket.com/me");
    ASSERT_THROW(r.getProperty("http://notthereuri"), std::runtime_error);

    ASSERT_EQ(nullptr, r.getOptionalProperty("http://notthereuri"));
}

TEST(_02_LoadSave, AllProperties) {
    Factory f;
    f.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/foafExample.rdf");

    Resource r = f.createIRIResource("http://jimmycricket.com/me");
    ASSERT_EQ(2, r.getPropertyValues("").size());
}

TEST(_02_LoadSave, findByType) {
    Factory f;
    f.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/foafExample.rdf");

    ResourceList list = f.findByType("http://xmlns.com/foaf/0.1/Person");
    for ( const Resource& res : list ) {
        std::cout << res << std::endl;
    }
    ASSERT_EQ(3, list.size());
}

TEST(_02_LoadSave, loadPerson) {
    Factory f;
    f.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/foafExample.rdf");

    ResourceList list = f.findByType("http://xmlns.com/foaf/0.1/Person");
    Resource person = *list.begin();
    for (const Property& p : person.getPropertyValues("")) {
        std::cout << p << std::endl;
    }
    ASSERT_EQ(14, person.getPropertyValues("").size());
}
