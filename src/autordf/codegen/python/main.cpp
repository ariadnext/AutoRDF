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
#include "KlassPython.h"
#include "UtilsPython.h"
#include "RdfsEntityPython.h"
#include <autordf/codegen/UtilsCommon.h>

namespace autordf {
namespace codegen {

bool verbose = false;

void run(Factory *f) {
    ontology::Ontology ontology(f, verbose);

    // Starting code Generation
    for (auto const& klassMapItem: ontology.classUri2Ptr()) {
        // created directory if needed
        auto pythKlass = KlassPython(*klassMapItem.second);
        createDirectory(pythKlass.genModuleInclusionPath());

        pythKlass.generateInterface();
        pythKlass.generate();

        std::ofstream ofs;
        createFile(pythKlass.genModuleInclusionPath() + "/__init__.py", &ofs, true);
        ofs << "from " << pythKlass.genModuleFullyQualified() << ".I" << klassMapItem.second->prettyIRIName() << " import I" << klassMapItem.second->prettyIRIName() << std::endl;
        ofs << "from " << pythKlass.genModuleFullyQualified() << "." << klassMapItem.second->prettyIRIName() << " import " << klassMapItem.second->prettyIRIName() << std::endl;
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
        autordf::codegen::RdfsEntityPython::preferredLang = vm["preferredLang"].as< std::vector<std::string> >();
    }

    if (vm.count("namespacemap")) {
        for (auto prefix_namespace: vm["namespacemap"].as< std::vector<std::string> >()) {
            std::stringstream ss(prefix_namespace);
            std::string prefix;
            std::getline(ss, prefix, ':');
            std::string ns;
            ss >> ns;
            if (autordf::codegen::verbose ) {
                std::cout << "Adding  " << prefix << "-->" << ns << " map." << std::endl;
            }
            f.addNamespacePrefix(prefix, ns);
        }
    }

    if (vm.count("outdir")) {
        autordf::codegen::RdfsEntityPython::outdir = vm["outdir"].as<std::string>();
    }

    if (!(vm.count("owlfile"))) {
        std::cerr << "Expected argument after options" << std::endl;
        return 1;
    }

    try {

        autordf::codegen::createDirectory(autordf::codegen::RdfsEntityPython::outdir);

        // Hardcode some prefixes
        f.addNamespacePrefix("owl", autordf::ontology::Ontology::OWL_NS);
        f.addNamespacePrefix("rdfs", autordf::ontology::Ontology::RDFS_NS);

        for (std::string owlfile: vm["owlfile"].as< std::vector<std::string> >() ) {
            if (autordf::codegen::verbose) {
                std::cout << "Loading " << owlfile << " into model." << std::endl;
            }
            f.loadFromFile(owlfile);
        }

        autordf::codegen::run(&f);

    } catch (const std::exception& e) {
        std::cerr << "E: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}
