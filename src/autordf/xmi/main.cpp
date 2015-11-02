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
        *out << "<ownedComment xmi:type=\"uml:Comment\" body=\"" << xmicomment << "\" annotatedElement=\"" << xmiid << "\"/>" << std::endl;
    }}

void run(Factory *f, const std::string& name) {
    ontology::Ontology ontology(f, verbose);

    *out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    *out << "<xmi:XMI xmi:version=\"2.1\" xmlns:uml=\"http://schema.omg.org/spec/UML/2.3\" xmlns:xmi=\"http://schema.omg.org/spec/XMI/2.1\">" << std::endl;
    *out <<     "<xmi:Documentation exporter=\"autordf\" />" << std::endl;
    *out <<     "<uml:Model xmi:type=\"uml:Model\" xmi:id=\"themodel\" name=\"" << name << "\">" << std::endl;
    *out <<         "<packagedElement xmi:type=\"uml:Package\" xmi:id=\"rdf\" name=\"" << name << "\">" << std::endl;

    for ( auto const& klassMapItem: ontology.classUri2Ptr() ) {
        const std::shared_ptr<ontology::Klass>& kls = klassMapItem.second;
        *out << "<packagedElement xmi:type=\"uml:Class\" name=\"" << kls->prettyIRIName() << "\"" << " xmi:id=\"" << kls->rdfname() << "\" visibility=\"public\">" << std::endl;
        genComment(*kls.get(),  kls->rdfname());
        for ( const std::shared_ptr<ontology::DataProperty>& dataProp: kls->dataProperties() ) {
            std::string xmiid = kls->rdfname() + "_" + dataProp->rdfname();
            *out << "<ownedAttribute xmi:type=\"uml:Property\" name=\"" << dataProp->prettyIRIName() << "\" xmi:id=\"" << xmiid << "\" visibility=\"public\">" << std::endl;
            genComment(*dataProp.get(), xmiid);
            *out << "</ownedAttribute>" << std::endl;
        }
        *out << "</packagedElement>" << std::endl;
        *out << std::endl;
    }

    *out <<         "</packagedElement>" << std::endl;
    *out <<     "</uml:Model>" << std::endl;
    *out << "</xmi:XMI>" << std::endl;
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
