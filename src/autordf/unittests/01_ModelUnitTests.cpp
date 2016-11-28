
#include <gtest/gtest.h>

#include <boost/filesystem.hpp>
#include <autordf/Uri.h>

#include "autordf/Model.h"

using namespace autordf;

TEST(_01_Model, SupportedFormats) {
    Model m;
    for ( auto f: m.supportedFormat() ) {
        std::cout << f << std::endl;
    }
}

TEST(_01_Model, Iterators) {
    Model ts;
    ts.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/example1.ttl");
    StatementList allStatements = ts.find();
    ASSERT_EQ(4, std::distance(allStatements.begin(), allStatements.end()));
    const StatementList& allStatementsConst = ts.find();
    ASSERT_EQ(4, std::distance(allStatementsConst.begin(), allStatementsConst.end()));
}

TEST(_01_Model, SeenPrefixes) {
    Model ts;
    ts.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/foafExample.ttl");
    for ( auto const& pair : ts.namespacesPrefixes() ) {
        std::cout << pair.first << ": " << pair.second << std::endl;
    }
    ASSERT_EQ("http://xmlns.com/foaf/0.1/", ts.prefixToNs("foaf") );
    ASSERT_EQ("foaf", ts.nsToPrefix("http://xmlns.com/foaf/0.1/") );
    ASSERT_EQ(3, ts.namespacesPrefixes().size());
}

TEST(_01_Model, SearchByPredicate) {
    Model ts;
    ts.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/foafExample.ttl");

    Statement req;
    req.predicate.setIri("http://xmlns.com/foaf/0.1/name");
    const StatementList& stmtList = ts.find(req);
    std::cout << stmtList << std::endl;
    ASSERT_EQ(3, stmtList.size());
}

TEST(_01_Model, SearchSubject) {
    Model ts;
    ts.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/example1.ttl");

    Node predicate;
    predicate.setIri("http://purl.org/dc/elements/1.1/title");
    Node object;
    object.setLiteral("RDF/XML Syntax Specification (Revised)");

    const NodeList& nodeList = ts.findSources(predicate, object);
    ASSERT_EQ(1, nodeList.size());
    ASSERT_STREQ("http://www.w3.org/TR/rdf-syntax-grammar", nodeList.begin()->iri());
}

TEST(_01_Model, SearchBySubject) {
    Model ts;
    ts.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/foafExample.ttl");

    Statement req;
    req.subject.setIri("http://jimmycricket.com/me");
    const StatementList& stmtList = ts.find(req);
    ASSERT_EQ(2, stmtList.size());
}

TEST(_01_Model, findTargets) {
    Model ts;
    ts.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/example1.ttl");

    Node source;
    source.setIri("http://www.w3.org/TR/rdf-syntax-grammar");
    Node predicate;
    predicate.setIri("http://purl.org/dc/elements/1.1/title");

    const NodeList& nodeList = ts.findTargets(source, predicate);
    ASSERT_EQ(1, nodeList.size());
    ASSERT_STREQ("RDF/XML Syntax Specification (Revised)", nodeList.begin()->literal());

    predicate.setIri("http://pwet");
    ASSERT_EQ(0, ts.findTargets(source, predicate).size());
}

TEST(_01_Model, findTarget) {
    Model ts;
    ts.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/foafExample.ttl");
    Node subject;
    subject.setIri("http://jimmycricket.com/me");
    Node predicate;
    predicate.setIri("http://xmlns.com/foaf/0.1/name");
    Node object = ts.findTarget(subject, predicate);
    ASSERT_STREQ("Jimmy Criket", object.literal());
}

TEST(_01_Model, SearchByObject) {
    Model ts;
    ts.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/foafExample.ttl");

    Statement req;
    req.object.setLiteral("Jimmy");
    const StatementList& stmtList = ts.find(req);
    ASSERT_EQ(1, stmtList.size());
}

TEST(_01_Model, AddSaveEraseStatement) {
    Model ts;
    Statement st;
    st.subject.setIri("http://mydomain/me");
    st.predicate.setIri("http://mydomain/firstName");
    st.object.setLiteral("Fabien");
    Statement stClone = st;

    ts.add(&st);
    ts.saveToFile("/tmp/test1.ttl");

    Model read1;
    read1.loadFromFile("/tmp/test1.ttl", "http://mydomain/me");
    const StatementList& stmtList = read1.find();
    ASSERT_EQ(1, stmtList.size());

    read1.remove(&stClone);
    const StatementList& stmtList1 = read1.find();
    ASSERT_EQ(0, stmtList1.size());
}

TEST(_01_Model, TypeLiterals) {
    Model ts;
    Statement st;
    st.subject.setIri("http://mydomain/me");
    st.predicate.setIri("http://mydomain/firstName");
    st.object.setLiteral("Fabien", "", "http://www.w3.org/2001/XMLSchema#string");
    ts.add(&st);

    st.subject.setIri("http://mydomain/me");
    st.predicate.setIri("http://mydomain/town");
    st.object.setLiteral("Fabien", "fr");
    ts.add(&st);

    const StatementList& stmtList = ts.find();

    std::cout << stmtList << std::endl;
}

TEST(DISABLED_01_Model, All) {
    Model ts;
    ts.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/foafExample.ttl");
    const StatementList& stmtList = ts.find();
    for(const Statement& stmt: stmtList) {
        std::cout << stmt << std::endl;
    }
}

TEST(_01_Model, QName) {
    Model ts;
    ts.loadFromFile(boost::filesystem::path(__FILE__).parent_path().string() + "/example2.ttl", "http://my/base/");

    ASSERT_STREQ("ex:editor", Uri("http://example.org/stuff/1.0/editor").QName(&ts).c_str());
    ASSERT_STREQ("truc", Uri("http://my/base/truc").QName(&ts).c_str());
    ASSERT_STREQ("http://out/of/base/or/prefixes", Uri("http://out/of/base/or/prefixes").QName(&ts).c_str());
}

TEST(_01_Model, LoadSaveMem) {
    Model ts;

    Statement st;
    st.subject.setIri("http://mydomain/me");
    st.predicate.setIri("http://mydomain/town");
    st.object.setLiteral("Fabien", "fr");
    ts.add(&st);

    std::shared_ptr<std::string> data = ts.saveToMemory("turtle");

    Model n;
    n.loadFromMemory(data->c_str(), "turtle");

    Statement st2;
    st2.subject.setIri("http://mydomain/me");
    st2.predicate.setIri("http://mydomain/town");
    st2.object.setLiteral("Fabien", "fr");
    n.remove(&st2);
}


TEST(_01_Model, LoadSave) {
    Model ts;

    Statement st;
    st.subject.setIri("http://mydomain/me");
    st.predicate.setIri("http://mydomain/town");
    st.object.setLiteral("Rennes", "fr");
    ts.add(&st);

    Statement st3;
    st3.subject.setIri("http://mydomain/me");
    st3.predicate.setIri("http://mydomain/Name");
    st3.object.setLiteral("Fabien", "fr");
    ts.add(&st3);

    ts.saveToFile("/tmp/autordf_unittest.ttl", "http://mydomain/me", true);
    ts.saveToFile("/tmp/autordf_unittest2.ttl", "http://mydomain/me", false);

    Model n;
    n.loadFromFile("/tmp/autordf_unittest.ttl", "http://mydomain/me");

    ASSERT_EQ(2, n.find().size());

    Statement st2;
    st2.subject.setIri("http://mydomain/me");
    st2.predicate.setIri("http://mydomain/town");
    st2.object.setLiteral("Rennes", "fr");
    n.remove(&st2);
}