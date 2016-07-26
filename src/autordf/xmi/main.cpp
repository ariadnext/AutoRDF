#include <getopt.h>

#include <iostream>
#include <fstream>
#include <set>
#include <map>
#include <memory>

#include <boost/algorithm/string.hpp>

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
        if ( kls->oneOfValues().empty() ) {
            umlType = "uml:Class";
        } else {
            umlType = "uml:Enumeration";
        }
        indent(out) << "<packagedElement xmi:type=\"" << umlType << "\" name=\"" << kls->prettyIRIName() << "\"" << " xmi:id=\"" << genXmiId(kls->rdfname()) << "\" visibility=\"public\">";
        pushIndent();
        genComment(*kls.get(), genXmiId(kls->rdfname()));
        for (const std::shared_ptr<ontology::Klass>& ancestor : kls->ancestors()) {
            std::string xmiid = kls->rdfname() + "_" + ancestor->rdfname();
            indent(out) << "<generalization xmi:type=\"uml:Generalization\" xmi:id=\"" << genXmiId(xmiid) << "\" general=\"" << genXmiId(ancestor->rdfname()) << "\"/>";
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
            std::string xmiid = genXmiId(kls->rdfname() + "_" + objectProp->rdfname());
            std::string assocxmiid = "Association" + genXmiId(kls->rdfname() + "_" + objectProp->rdfname());
            std::string assocendxmiid = "AssociationEnd" + genXmiId(kls->rdfname() + "_" + objectProp->rdfname());
            std::string typexmiid = genXmiId(objectProp->range(kls.get()));
            if ( typexmiid.empty() ) {
                typexmiid = genXmiId(ontology::Ontology::OWL_NS + "Thing");
            }

            indent(out) << "<ownedAttribute xmi:type=\"uml:Property\" name=\"" << objectProp->prettyIRIName()
            << "\" xmi:id=\"" << xmiid << "\" visibility=\"public\""
            << " association=\"" << assocxmiid << "\" aggregation=\"none\">";
            pushIndent();
            indent(out) << "<type xmi:type=\"uml:Class\" xmi:idref=\"" << typexmiid << "\"/>";
            genComment(*objectProp.get(), genXmiId(xmiid));
            popIndent();
            indent(out) << "</ownedAttribute>";

            indent(out) << "<packagedElement xmi:type=\"uml:Association\" xmi:id=\"" << assocxmiid << "\" visibility=\"public\">";
            pushIndent();
            indent(out) << "<memberEnd xmi:idref=\"" << xmiid << "\"/>";
            indent(out) << "<ownedEnd xmi:type=\"uml:Property\" xmi:id=\"" << assocendxmiid << "\" visibility=\"public\" type=\"" << kls->rdfname() << "\" aggregation=\"none\" isNavigable=\"false\"/>";
            popIndent();
            indent(out) << "</packagedElement>";
        }
        popIndent();
        indent(out) << "</packagedElement>";
        *out << std::endl;
    }

    *out << std::endl;
    popIndent();
    indent(out) << "</packagedElement>";
    popIndent();
    indent(out) << "</uml:Model>";
    popIndent();
    indent(out) << "</xmi:XMI>";
}

int main(int argc, char **argv) {
    autordf::Factory f;

    std::stringstream usage;
    usage << "Usage: " << argv[0] << " [-v] [-n namespacemap] [-o outfile] owlfile1 [owlfile2...]\n";
    usage << "\t" << "Processes an OWL file, and generates UML XMI file from it in current directory\n";
    usage << "\t" << "namespacemap:\t Adds supplementary namespaces prefix definition, in the form 'prefix:namespace IRI'. Defaults to empty.\n";
    usage << "\t" << "outdir:\t Folder where to generate files in. If it does not exit it will be created. Defaults to current directory." << ".\n";
    usage << "\t" << "-v:\t Turn verbose output on." << ".\n";
    int opt;
    while ((opt = ::getopt(argc, argv, "vhn:o:")) != -1) {
        switch (opt) {
            case 'n': {
                {
                    std::stringstream ss(optarg);
                    std::string prefix;
                    std::getline(ss, prefix, ':');
                    std::string ns;
                    ss >> ns;
                    if ( verbose ) {
                        std::cout << "Adding  " << prefix << "-->" << ns << " map." << std::endl;
                    }
                    f.addNamespacePrefix(prefix, ns);
                }
                break;
            }
            case 'o':
            {
                //FIXME handle errors
                ofs.open(optarg);
                out = &ofs;
                break;
            }
            case 'v':
                verbose = true;
                break;
            case 'h':
            default: /* '?' */
                std::cerr << usage.str();
                std::cerr.flush();
                return 1;
        }
    }

    if (optind >= argc) {
        std::cerr << "Expected argument after options" << std::endl;
        return 1;
    }

    // Hardcode some prefixes
    f.addNamespacePrefix("owl", autordf::ontology::Ontology::OWL_NS);
    f.addNamespacePrefix("rdfs", autordf::ontology::Ontology::RDFS_NS);
    //FIXME: Read that from command line
    std::string baseURI = "http://";

    std::string firstFile;
    while ( optind < argc ) {
        if ( verbose ) {
            std::cout << "Loading " << argv[optind] << " into model." << std::endl;
        }
        if ( firstFile.empty() ) {
            firstFile = argv[optind];
        }
        f.loadFromFile(argv[optind], baseURI);
        optind++;
    }

    run(&f, firstFile);
    return 0;
}
