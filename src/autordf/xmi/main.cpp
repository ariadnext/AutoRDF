#include <iostream>
#include <fstream>
#include <set>
#include <map>
#include <memory>

#include <boost/algorithm/string.hpp>

#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>

#include <autordf/Factory.h>
#include <autordf/Object.h>
#include <autordf/ontology/Ontology.h>

using namespace autordf;

std::ofstream ofs;
std::ostream* out = &std::cout;
bool verbose;

void escape(std::string *data)
{
    using boost::algorithm::replace_all;
    replace_all(*data, "&",  "&amp;");
    replace_all(*data, "\"", "&quot;");
    replace_all(*data, "\'", "&apos;");
    replace_all(*data, "<",  "&lt;");
    replace_all(*data, ">",  "&gt;");
}

// Builds and id that does not contain any non strange char, as modelio chokes on it
std::string genXmiId(const std::string& rawData) {
    static std::list<std::string> table;
    unsigned int id = 0;
    bool found = false;
    for ( const std::string& item : table) {
        if ( item == rawData ) {
            found = true;
            break;
        }
        ++id;
    }

    if ( !found ) {
        table.push_back(rawData);
        id = table.size() - 1;
    }

    std::stringstream ss;
    ss << "_" << (id + 1);

    return ss.str();
}

int _indent = 0;
std::ostream& indent(std::ostream *stream) {
    *stream << std::endl;
    for ( unsigned int i = 0; i < _indent; ++i ) {
        *stream << "    ";
    }
    return *stream;
}

void pushIndent() {
    ++_indent;
}
void popIndent() {
    --_indent;
    if ( _indent < 0 ) {
        throw std::runtime_error("_indent < 0");
    }
}

void genComment(ontology::RdfsEntity& entity, const std::string& xmiid) {
    if ( !entity.label().empty() || !entity.comment().empty() ) {
        std::string xmicomment;
        if ( !entity.label().empty() ) {
            xmicomment = entity.label();
        }
        if ( !entity.comment().empty() ) {
            xmicomment.append("\n");
            xmicomment.append(entity.comment());
        }
        escape(&xmicomment);
        indent(out) << "<ownedComment xmi:type=\"uml:Comment\" body=\"" << xmicomment << "\" annotatedElement=\"" << xmiid << "\"/>";
    }
}

class Association {
public:
    std::string id;
    std::string memberEndId;
    std::string ownedEndId;
    std::string type;
};

std::vector<Association> _associations;

void run(Factory *f, const std::string& name) {
    ontology::Ontology ontology(f, verbose);

    *out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    indent(out) << "<xmi:XMI xmi:version=\"2.1\" xmlns:uml=\"http://schema.omg.org/spec/UML/2.3\" xmlns:xmi=\"http://schema.omg.org/spec/XMI/2.1\">";
    pushIndent();
    indent(out) << "<xmi:Documentation exporter=\"autordf\" />";
    indent(out) << "<uml:Model xmi:type=\"uml:Model\" xmi:id=\"themodel\" name=\"" << name << "\">";
    pushIndent();
    indent(out) << "<packagedElement xmi:type=\"uml:Package\" xmi:id=\"rdf\" name=\"" << name << "\">";
    *out << std::endl;

    pushIndent();
    for ( auto const& klassMapItem: ontology.classUri2Ptr() ) {
        const std::shared_ptr<ontology::Klass>& kls = klassMapItem.second;
        std::string umlType;
        bool isAnEnum = !kls->oneOfValues().empty();
        if ( isAnEnum ) {
            umlType = "uml:Enumeration";
        } else {
            umlType = "uml:Class";
        }
        indent(out) << "<packagedElement xmi:type=\"" << umlType << "\" name=\"" << kls->prettyIRIName() << "\"" << " xmi:id=\"" << genXmiId(kls->rdfname()) << "\" visibility=\"public\">";
        pushIndent();
        genComment(*kls.get(), genXmiId(kls->rdfname()));
        if ( !isAnEnum ) {
            for (const std::shared_ptr<ontology::Klass>& ancestor : kls->ancestors()) {
                std::string xmiid = kls->rdfname() + "_" + ancestor->rdfname();
                indent(out) << "<generalization xmi:type=\"uml:Generalization\" xmi:id=\"" << genXmiId(xmiid) << "\" general=\"" << genXmiId(ancestor->rdfname()) << "\"/>";
            }
        }
        for ( const std::shared_ptr<ontology::DataProperty>& dataProp: kls->dataProperties() ) {
            std::string xmiid = genXmiId(kls->rdfname() + "_" + dataProp->rdfname());
            indent(out) << "<ownedAttribute xmi:type=\"uml:Property\" name=\"" << dataProp->prettyIRIName() << "\" xmi:id=\"" << genXmiId(xmiid) << "\" visibility=\"public\">";
            pushIndent();
            genComment(*dataProp.get(), xmiid);
            popIndent();
            indent(out) << "</ownedAttribute>";
        }
        for ( const std::shared_ptr<ontology::ObjectProperty>& objectProp: kls->objectProperties() ) {
            Association a;
            a.id = "Association" + genXmiId(kls->rdfname() + "_" + objectProp->rdfname());
            a.memberEndId = genXmiId(kls->rdfname() + "_" + objectProp->rdfname());
            a.ownedEndId = "AssociationEnd" + genXmiId(kls->rdfname() + "_" + objectProp->rdfname());
            a.type = genXmiId(kls->rdfname());

            std::string typexmiid = genXmiId(objectProp->range(kls.get()));
            if ( typexmiid.empty() ) {
                typexmiid = genXmiId(ontology::Ontology::OWL_NS + "Thing");
            }

            indent(out) << "<ownedAttribute xmi:type=\"uml:Property\" name=\"" << objectProp->prettyIRIName()
            << "\" xmi:id=\"" << a.memberEndId << "\" visibility=\"public\""
            << " association=\"" << a.id << "\" aggregation=\"none\">";
            pushIndent();
            indent(out) << "<type xmi:type=\"uml:Class\" xmi:idref=\"" << typexmiid << "\"/>";
            genComment(*objectProp.get(), genXmiId(a.memberEndId));
            popIndent();
            indent(out) << "</ownedAttribute>";

            _associations.push_back(a);
        }

        popIndent();
        indent(out) << "</packagedElement>";
        *out << std::endl;
    }

    *out << std::endl;
    for ( const Association& a : _associations ) {
        indent(out) << "<packagedElement xmi:type=\"uml:Association\" xmi:id=\"" << a.id << "\" visibility=\"public\">";
        pushIndent();
        indent(out) << "<memberEnd xmi:idref=\"" << a.memberEndId << "\"/>";
        indent(out) << "<ownedEnd xmi:type=\"uml:Property\" xmi:id=\"" << a.ownedEndId << "\" visibility=\"public\" type=\"" << a.type << "\" association=\"" << a.id << "\"/>";
        popIndent();
        indent(out) << "</packagedElement>";
    }

    popIndent();
    indent(out) << "</packagedElement>";
    popIndent();
    indent(out) << "</uml:Model>";
    popIndent();
    indent(out) << "</xmi:XMI>";
}

int main(int argc, char **argv) {
    autordf::Factory f;

    namespace po = boost::program_options;

    po::options_description desc("Usage: autordfxmi [-v] [-n namespacemap] [-o outfile] owlfile1 [owlfile2...]\n"
                                 "Processes an OWL file, and generates UML XMI file from it in current directory");

    desc.add_options()
            ("help,h", "Describe arguments")
            ("verbose,v", "Turn verbose output on.")
            ("all-in-one,a", "Generate one cpp file that includes all the other called AllInOne.cpp")
            ("namespacemap,n", po::value< std::vector<std::string> >(), "Adds supplementary namespaces prefix definition, in the form 'prefix:namespace IRI'. Defaults to empty.")
            ("outfile,o", po::value< std::string >(), "File to write to.")
            ("owlfile", po::value< std::vector<std::string> >(), "Input file (repeated)");

    po::positional_options_description p;
    p.add("owlfile", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
            options(desc).positional(p).run(), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 1;
    }

    verbose = vm.count("verbose") > 0;

    if(vm.count("namespacemap")) {
        for(auto ns: vm["namespacemap"].as< std::vector<std::string> >()) {
            std::stringstream ss(ns);
            std::string prefix;
            std::getline(ss, prefix, ':');
            std::string ns;
            ss >> ns;
            if (verbose) {
                std::cout << "Adding  " << prefix << "-->" << ns << " map." << std::endl;
            }
            f.addNamespacePrefix(prefix, ns);
        }
    }

    if(vm.count("outfile")) {
        //FIXME handle errors
        ofs.open(vm["outfile"].as<std::string>());
        out = &ofs;
    }

    if (!(vm.count("owlfile"))) {
        std::cerr << "Expected argument after options" << std::endl;
        return 1;
    }

    // Hardcode some prefixes
    f.addNamespacePrefix("owl", autordf::ontology::Ontology::OWL_NS);
    f.addNamespacePrefix("rdfs", autordf::ontology::Ontology::RDFS_NS);
    //FIXME: Read that from command line
    std::string baseURI = "http://";

    try {

        std::string firstFile;
        for ( std::string owlfile: vm["owlfile"].as< std::vector<std::string> >() ) {
            if ( verbose ) {
                std::cout << "Loading " << owlfile << " into model." << std::endl;
            }
            if ( firstFile.empty() ) {
                firstFile = owlfile;
            }
            f.loadFromFile(owlfile, baseURI);
        }
    
        run(&f, firstFile);
        return 0;

    } catch(const std::exception& e) {
        std::cerr << "E: " << e.what() << std::endl;
        return -1;
    }
}
