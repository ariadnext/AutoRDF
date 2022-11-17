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

#include "Environment.h"
#include "cpp/CppCodeGenerator.h"
#include "python/PythonCodeGenerator.h"

int main(int argc, char** argv) {
    autordf::Factory f;

    namespace po = boost::program_options;

    po::options_description desc("Usage: autordfcodegen [-v] [-a] [-n namespacemap] [-o outdir] owlfile1 [owlfile2...]\n"
                                 "\tProcesses an OWL file, and generates C++ classes from it in current directory\n");

    desc.add_options()
            ("help,h", "Describe arguments")
            ("verbose,v", "Turn verbose output on.")
            ("generator,g", po::value< std::string >()->default_value("cpp"), "Choose the generator to be used (supported: cpp, python; default: cpp).")
            ("all-in-one,a", "Generate one cpp file that includes all the other called AllInOne.cpp (cpp only)")
            ("namespacemap,n", po::value< std::vector<std::string> >(), "Adds supplementary namespaces prefix definition, in the form 'prefix:namespace IRI'. Defaults to empty.")
            ("outdir,o", po::value< std::string >(), "Folder where to generate files in. If it does not exit it will be created. Defaults to current directory.")
            ("owlfile", po::value< std::vector<std::string> >(), "Input file (repeated)")
            ("preferredLang,l", po::value< std::vector<std::string> >(), "Preferred languages for documentation (repeated)")
            ("tpldir,t", po::value< std::string >(), "Folder containing the template files. Defaults to 'template' inside the current directory.");

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

    autordf::codegen::Environment::verbose = vm.count("verbose") > 0;
    auto generateAllInOne = vm.count("all-in-one") > 0;;

    if (vm.count("preferredLang")) {
        autordf::codegen::Environment::preferredLang = vm["preferredLang"].as< std::vector<std::string> >();
    }

    if(vm.count("namespacemap")) {
        for(auto prefix_namespace: vm["namespacemap"].as< std::vector<std::string> >()) {
            std::stringstream ss(prefix_namespace);
            std::string prefix;
            std::getline(ss, prefix, ':');
            std::string ns;
            ss >> ns;
            if ( autordf::codegen::Environment::verbose ) {
                std::cout << "Adding  " << prefix << "-->" << ns << " map." << std::endl;
            }
            f.addNamespacePrefix(prefix, ns);
        }
    }

    if(vm.count("tpldir")) {
        autordf::codegen::Environment::tpldir = vm["tpldir"].as<std::string>();
        if (autordf::codegen::Environment::tpldir.back() != '/') {
            autordf::codegen::Environment::tpldir += '/';
        }

        if (autordf::codegen::Environment::verbose) {
            std::cout << "Template directory: " << autordf::codegen::Environment::tpldir << std::endl;
        }
    }

    if(vm.count("outdir")) {
        autordf::codegen::Environment::outdir = vm["outdir"].as<std::string>();

        if (autordf::codegen::Environment::verbose) {
            std::cout << "Output directory: " << autordf::codegen::Environment::outdir << std::endl;
        }
    }


    if (!(vm.count("owlfile"))) {
        std::cerr << "Expected argument after options" << std::endl;
        return 1;
    }

    std::unique_ptr<autordf::codegen::CodeGenerator> generator;
    try {
        autordf::codegen::Environment::createDirectory(autordf::codegen::Environment::outdir);

        // Hardcode some prefixes
        f.addNamespacePrefix("owl", autordf::ontology::Ontology::OWL_NS);
        f.addNamespacePrefix("rdfs", autordf::ontology::Ontology::RDFS_NS);

        for ( std::string owlfile: vm["owlfile"].as< std::vector<std::string> >() ) {
            if ( autordf::codegen::Environment::verbose ) {
                std::cout << "Loading " << owlfile << " into model." << std::endl;
            }
            f.loadFromFile(owlfile);
        }

        auto generatorStr = vm["generator"].as<std::string>();
        if (generatorStr == "cpp") {
            generator = std::unique_ptr<autordf::codegen::CodeGenerator>(new autordf::codegen::cpp::CppCodeGenerator(&f, generateAllInOne));
        } else if (generatorStr == "python") {
            generator = std::unique_ptr<autordf::codegen::CodeGenerator>(new autordf::codegen::python::PythonCodeGenerator(&f));
        } else {
            std::cerr << "Invalid generator provided: " << generatorStr << std::endl;
            return 1;
        }

        generator->run();
    } catch(const std::exception& e) {
        std::cerr << "E: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}
