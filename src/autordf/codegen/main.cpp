#include <getopt.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <set>
#include <map>
#include <memory>

#include <autordf/Factory.h>
#include <autordf/Object.h>
#include <autordf/ontology/Ontology.h>

#include "DataProperty.h"
#include "ObjectProperty.h"
#include "Klass.h"
#include "Utils.h"

namespace autordf {
namespace codegen {

bool verbose = false;
bool generateAllInOne = false;

void generateRdfTypeInfo() {
    std::ofstream oifs;
    createFile(RdfsEntity::outdir + "/RdfTypeInfo.h", &oifs);
    generateCodeProtectorBegin(oifs, "", "RdfTypeInfo");
    oifs << "class RdfTypeInfo {" << std::endl;
    oifs << "public:" << std::endl;
    indent(oifs, 1) << "RdfTypeInfo();" << std::endl;
    oifs << std::endl;
    indent(oifs, 1) << "static const std::map<std::string, std::set<std::string> >& data() { return DATA; }" << std::endl;
    oifs << "private:" << std::endl;
    indent(oifs, 1) << "static std::map<std::string, std::set<std::string> > DATA;" << std::endl;
    oifs << "};" << std::endl;
    oifs << std::endl;
    generateCodeProtectorEnd(oifs, "", "RdfTypeInfo");

    std::ofstream ofs;
    createFile(RdfsEntity::outdir + "/RdfTypeInfo.cpp", &ofs);
    addBoilerPlate(ofs);
    ofs << std::endl;
    ofs << "#include <map>" << std::endl;
    ofs << "#include <set>" << std::endl;
    ofs << "#include <string>" << std::endl;
    ofs << std::endl;
    if ( RdfsEntity::outdir == ".") {
        ofs << "#include \"RdfTypeInfo.h\"" << std::endl;
    } else {
        ofs << "#include \"" << RdfsEntity::outdir << "/RdfTypeInfo.h\"" << std::endl;
    }
    ofs << std::endl;
    for ( auto const& klassMapItem: ontology::Klass::uri2Ptr) {
        const Klass cls(*klassMapItem.second);
        ofs << "#include \"" << cls.genCppNameSpaceInclusionPath() << "/" << klassMapItem.second->prettyIRIName() << ".h" << "\"" << std::endl;
    }
    ofs << std::endl;

    ofs << "std::map<std::string, std::set<std::string> > RdfTypeInfo::DATA;" << std::endl;
    ofs << std::endl;
    ofs << "RdfTypeInfo::RdfTypeInfo() {" << std::endl;
    indent(ofs, 1) << "if ( DATA.empty() ) {" << std::endl;
    for ( auto const& klassMapItem: ontology::Klass::uri2Ptr) {
        const Klass& cls = *klassMapItem.second;
        indent(ofs, 2) << "DATA[\"" << klassMapItem.first << "\"] = " << cls.genCppNameSpaceFullyQualified() << "::" <<
                klassMapItem.second->prettyIRIName() << "::ancestorsRdfTypeIRI();" << std::endl;
    }
    indent(ofs, 1) << "};" << std::endl;
    ofs << std::endl;
    ofs << "}" << std::endl;
    ofs << std::endl;
    ofs << "namespace {" << std::endl;
    ofs << "RdfTypeInfo __loader;" << std::endl;
    ofs << "}" << std::endl;
}

void run(Factory *f) {
    ontology::Ontology::populateSchemaClasses(f);

    // Starting code Generation
    std::set<std::string> cppNameSpaces;

    for ( auto const& klassMapItem: ontology::Klass::uri2Ptr) {
        // created directory if needed
        createDirectory(Klass(*klassMapItem.second).genCppNameSpaceInclusionPath());
        cppNameSpaces.insert(Klass(*klassMapItem.second).genCppNameSpace());

        Klass(*klassMapItem.second).generateInterfaceDeclaration();
        Klass(*klassMapItem.second).generateInterfaceDefinition();
        Klass(*klassMapItem.second).generateDeclaration();
        Klass(*klassMapItem.second).generateDefinition();
    }

    // Generate all TypesInfo
    generateRdfTypeInfo();

    // Generate all inclusions files
    for ( const std::string& cppNameSpace : cppNameSpaces ) {
        std::ofstream ofs;
        createFile(Klass::outdir + "/" + cppNameSpace + "/" + cppNameSpace + ".h", &ofs);

        generateCodeProtectorBegin(ofs, cppNameSpace, cppNameSpace);
        for ( auto const& klassMapItem: ontology::Klass::uri2Ptr) {
            if ( Klass(*klassMapItem.second).genCppNameSpace() == cppNameSpace ) {
                const Klass& cls = *klassMapItem.second;
                ofs << "#include <" << cls.genCppNameSpaceInclusionPath() << "/" << klassMapItem.second->prettyIRIName() << ".h" << ">" << std::endl;
            }
        }
        ofs << std::endl;
        generateCodeProtectorEnd(ofs, cppNameSpace, cppNameSpace);
    }

    // Generate all in one cpp file
    if ( generateAllInOne ) {
        std::ofstream ofs;
        createFile(RdfsEntity::outdir + "/AllInOne.cpp", &ofs);

        addBoilerPlate(ofs);
        ofs << std::endl;
        ofs << "#include \"RdfTypeInfo.cpp\"" << std::endl;
        for ( auto const& klassMapItem: ontology::Klass::uri2Ptr) {
            const Klass& cls = *klassMapItem.second;
            ofs << "#include \"" << cls.genCppNameSpaceInclusionPath() << "/I" << klassMapItem.second->prettyIRIName() << ".cpp" << "\"" << std::endl;
            ofs << "#include \"" << cls.genCppNameSpaceInclusionPath() << "/" << klassMapItem.second->prettyIRIName() << ".cpp" << "\"" << std::endl;
        }
        ofs << std::endl;
    }
}

}
}

int main(int argc, char **argv) {
    autordf::Factory f;

    std::stringstream usage;
    usage << "Usage: " << argv[0] << " [-v] [-a] [-n namespacemap] [-o outdir] owlfile1 [owlfile2...]\n";
    usage << "\t" << "Processes an OWL file, and generates C++ classes from it in current directory\n";
    usage << "\t" << "namespacemap:\t Adds supplementary namespaces prefix definition, in the form 'prefix:namespace IRI'. Defaults to empty.\n";
    usage << "\t" << "outdir:\t Folder where to generate files in. If it does not exit it will be created. Defaults to current directory." << ".\n";
    usage << "\t" << "-v:\t Turn verbose output on." << ".\n";
    usage << "\t" << "-a:\t Generate one cpp file that includes all the other called AllInOne.cpp" << ".\n";
    int opt;
    while ((opt = ::getopt(argc, argv, "avhn:o:")) != -1) {
        switch (opt) {
            case 'n': {
                {
                    std::stringstream ss(optarg);
                    std::string prefix;
                    std::getline(ss, prefix, ':');
                    std::string ns;
                    ss >> ns;
                    if ( autordf::codegen::verbose ) {
                        std::cout << "Adding  " << prefix << "-->" << ns << " map." << std::endl;
                    }
                    f.addNamespacePrefix(prefix, ns);
                }
                break;
            }
            case 'o':
                autordf::codegen::RdfsEntity::outdir = optarg;
                break;
            case 'v':
                autordf::codegen::verbose = true;
                break;
            case 'a':
                autordf::codegen::generateAllInOne = true;
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

    autordf::codegen::createDirectory(autordf::codegen::RdfsEntity::outdir);

    // Hardcode some prefixes
    f.addNamespacePrefix("owl", autordf::ontology::Ontology::OWL_NS);
    f.addNamespacePrefix("rdfs", autordf::ontology::Ontology::RDFS_NS);
    //FIXME: Read that from command line
    std::string baseURI = "http://";

    while ( optind < argc ) {
        if ( autordf::codegen::verbose ) {
            std::cout << "Loading " << argv[optind] << " into model." << std::endl;
        }
        f.loadFromFile(argv[optind], baseURI);
        optind++;
    }
    autordf::codegen::run(&f);
    return 0;
}
