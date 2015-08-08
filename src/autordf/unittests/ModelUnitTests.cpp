
#include <gtest/gtest.h>

#include <boost/filesystem.hpp>

#include "autordf/Model.h"

using namespace autordf;

TEST(Model, Iterator) {
    Model ts;
    ts.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/example1.ttl");
    StatementList allStatements = ts.find();
    ASSERT_EQ(5, std::distance(allStatements.begin(), allStatements.end()));
}

TEST(Model, ConstIterator) {
    Model ts;
    ts.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/example1.ttl");
    const StatementList& allStatements = ts.find();
    ASSERT_EQ(5, std::distance(allStatements.begin(), allStatements.end()));
}
