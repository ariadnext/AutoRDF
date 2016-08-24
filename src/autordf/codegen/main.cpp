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

#include "codegen/DataProperty.h"
#include "codegen/ObjectProperty.h"
#include "codegen/Klass.h"
#include "codegen/Utils.h"

namespace autordf {
namespace codegen {

bool verbose = false;
bool generateAllInOne = false;

void run(Factory *f) {
    ontology::Ontology ontology(f, verbose);

    // Starting code Generation
    std::set<std::string> cppNameSpaces;

    for ( auto const& klassMapItem: ontology.classUri2Ptr()) {
        // created directory if needed
        createDirectory(Klass(*klassMapItem.second).genCppNameSpaceInclusionPath());
        cppNameSpaces.insert(Klass(*klassMapItem.second).genCppNameSpace());

        Klass(*klassMapItem.second).generateInterfaceDeclaration();
        Klass(*klassMapItem.second).generateInterfaceDefinition();
        Klass(*klassMapItem.second).generateDeclaration();
        Klass(*klassMapItem.second).generateDefinition();
    }

    // Generate all inclusions files
    for ( const std::string& cppNameSpace : cppNameSpaces ) {
        std::ofstream ofs;
        createFile(Klass::outdir + "/" + cppNameSpace + "/" + cppNameSpace + ".h", &ofs);

        generateCodeProtectorBegin(ofs, cppNameSpace, cppNameSpace);
        for ( auto const& klassMapItem: ontology.classUri2Ptr()) {
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
        for ( auto const& klassMapItem: ontology.classUri2Ptr()) {
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

    while ( optind < argc ) {
        if ( autordf::codegen::verbose ) {
            std::cout << "Loading " << argv[optind] << " into model." << std::endl;
        }
        f.loadFromFile(argv[optind]);
        optind++;
    }
    autordf::codegen::run(&f);
    return 0;
}
