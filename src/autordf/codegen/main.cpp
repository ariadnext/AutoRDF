#include <iostream>
#include <sstream>
#include <fstream>
#include <set>
#include <map>
#include <memory>

#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>

#include <autordf/Factory.h>
#include <autordf/Object.h>
#include <autordf/ontology/Ontology.h>

#include "codegen/DataProperty.h"
#include "codegen/ObjectProperty.h"
#include "codegen/Klass.h"
#include "codegen/Utils.h"
#include "codegen/RdfsEntity.h"

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

    namespace po = boost::program_options;

    po::options_description desc("Usage: autordfcodegen [-v] [-a] [-n namespacemap] [-o outdir] owlfile1 [owlfile2...]\n"
                                 "\tProcesses an OWL file, and generates C++ classes from it in current directory\n");

    desc.add_options()
            ("help,h", "Describe arguments")
            ("verbose,v", "Turn verbose output on.")
            ("all-in-one,a", "Generate one cpp file that includes all the other called AllInOne.cpp")
            ("namespacemap,n", po::value< std::vector<std::string> >(), "Adds supplementary namespaces prefix definition, in the form 'prefix:namespace IRI'. Defaults to empty.")
            ("outdir,o", po::value< std::string >(), "Folder where to generate files in. If it does not exit it will be created. Defaults to current directory.")
            ("owlfile", po::value< std::vector<std::string> >(), "Input file (repeated)")
            ("preferredLang,l", po::value< std::vector<std::string> >(), "Preferred languages for documentation (repeated)");

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

    autordf::codegen::verbose = vm.count("verbose") > 0;

    if (vm.count("preferredLang")) {
        autordf::codegen::RdfsEntity::preferredLang = vm["preferredLang"].as< std::vector<std::string> >();
    }

    if(vm.count("namespacemap")) {
        for(auto prefix_namespace: vm["namespacemap"].as< std::vector<std::string> >()) {
            std::stringstream ss(prefix_namespace);
            std::string prefix;
            std::getline(ss, prefix, ':');
            std::string ns;
            ss >> ns;
            if ( autordf::codegen::verbose ) {
                std::cout << "Adding  " << prefix << "-->" << ns << " map." << std::endl;
            }
            f.addNamespacePrefix(prefix, ns);
        }
    }

    if(vm.count("outdir")) {
        autordf::codegen::RdfsEntity::outdir = vm["outdir"].as<std::string>();
    }

    autordf::codegen::generateAllInOne = vm.count("all-in-one") > 0;;

    if (!(vm.count("owlfile"))) {
        std::cerr << "Expected argument after options" << std::endl;
        return 1;
    }

    try {

        autordf::codegen::createDirectory(autordf::codegen::RdfsEntity::outdir);
    
        // Hardcode some prefixes
        f.addNamespacePrefix("owl", autordf::ontology::Ontology::OWL_NS);
        f.addNamespacePrefix("rdfs", autordf::ontology::Ontology::RDFS_NS);
    
        for ( std::string owlfile: vm["owlfile"].as< std::vector<std::string> >() ) {
            if ( autordf::codegen::verbose ) {
                std::cout << "Loading " << owlfile << " into model." << std::endl;
            }
            f.loadFromFile(owlfile);
        }
    
        autordf::codegen::run(&f);

    } catch(const std::exception& e) {
        std::cerr << "E: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}
