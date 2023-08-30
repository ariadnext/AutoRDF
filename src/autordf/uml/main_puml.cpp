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
#include <autordf/I18StringVector.h>

using namespace autordf;


class Association {
public:
    std::string source;
    std::string dest;
    std::string type;

    bool operator==(const Association& rhs) const {
        return source == rhs.source &&
               dest == rhs.dest &&
               type == rhs.type;
    }
};

// Escape special characters
void escape(std::string *data)
{
    using boost::algorithm::replace_all;
    replace_all(*data, "&",  "&amp;");
    replace_all(*data, "\"", "&quot;");
    replace_all(*data, "\'", "&apos;");
    replace_all(*data, "<",  "&lt;");
    replace_all(*data, ">",  "&gt;");
}

std::string generateComment(ontology::RdfsEntity& entity, const std::vector<std::string> & preferredLang) {
    std::string comment;
    if ( !entity.label().empty() || !entity.comment().empty() ) {
        if ( !entity.label().empty() ) {
            try {
                autordf::I18StringVector langString(entity.label());
                comment = langString.langPreferenceString(preferredLang);
            } catch (DataConvertionFailure&) {
                comment = entity.label().front();
            }
        }
        if ( !entity.comment().empty() ) {
            comment.append("\n");
            try {
                autordf::I18StringVector langString(entity.label());
                comment.append(langString.langPreferenceString(preferredLang));
            } catch (DataConvertionFailure&) {
                comment.append(entity.label().front());
            }
        }
        escape(&comment);
    }
    return comment;
}

std::string computeCardinality(const unsigned int& min, const unsigned int& max) {
    if (min == max) {
        return std::to_string(min);
    }
    std::string ret = std::to_string(min) + "..";
    if (max == std::numeric_limits<unsigned int>::max()) {
        ret += "n";
    }
    else {
        ret += std::to_string(max);
    }
    return ret;
}


void run(Factory *f, bool verbose, bool withNote, const std::vector<std::string> & preferredLang, std::ostream* out) {
    std::vector<Association> associations;

    ontology::Ontology ontology(f, verbose);
    *out << "@startuml" << std::endl;
    *out << "!pragma layout smetana" << std::endl;

    for ( auto const& klassMapItem: ontology.classUri2Ptr() ) {
        const std::shared_ptr<ontology::Klass>& kls = klassMapItem.second;
        if (kls->prettyIRIName() == "Thing" || kls->prettyIRIName() == "Resource") {
            continue;
        }
        std::string classType;
        bool isAnEnum = !kls->oneOfValues().empty();
        if ( isAnEnum ) {
            classType = "enum";
        } else {
            classType = "class";
        }
        *out << classType << " " << kls->prettyIRIName() << "{" << std::endl;

        if ( !isAnEnum ) {
            for (const std::shared_ptr<ontology::Klass>& ancestor : kls->ancestors()) {
                if (ancestor->prettyIRIName() == "Thing") {
                    continue;
                }
                Association a;
                a.type = "-up-|>";
                a.source = kls->prettyIRIName();
                a.dest = ancestor->prettyIRIName();
                associations.push_back(a);
            }
            for ( const std::shared_ptr<ontology::DataProperty>& dataProp: kls->dataProperties() ) {
                *out << "+" << dataProp->prettyIRIName() << " : " << dataProp->range(kls.get()).prettyName() << " [" << computeCardinality(dataProp->minCardinality(*kls), dataProp->maxCardinality(*kls)) << "]" << std::endl;
            }
            for ( const std::shared_ptr<ontology::ObjectProperty>& objectProp: kls->objectProperties() ) {
                *out << "+" << objectProp->prettyIRIName() << "() : " << objectProp->range(kls.get()).prettyName() << " [" << computeCardinality(objectProp->minCardinality(*kls), objectProp->maxCardinality(*kls)) << "]"  << std::endl;
                Association a;
                a.type = "---->";
                a.source = kls->prettyIRIName();
                a.dest = objectProp->range(kls.get()).prettyName();
                associations.push_back(a);

                associations.push_back(a);
            }
        }
        else {
            for ( const auto& enumValue: kls->oneOfValues() ) {
                *out << enumValue.prettyIRIName() << std::endl;
            }
        }

        *out << "}" <<std::endl;
        std::string comment = generateComment(*kls, preferredLang);
        if (withNote && !comment.empty())
        {
            *out << "note left of " << kls->prettyIRIName() << " : " << generateComment(*kls, preferredLang) << std::endl;
        }
    }
    auto it = std::unique(associations.begin(), associations.end());
    associations.erase(it, associations.end());
    for ( const Association& a : associations ) {
        *out << a.source << " " << a.type << " " << a.dest << std::endl;
    }

    *out << "@enduml";
}

int main(int argc, char **argv) {
    autordf::Factory f;

    namespace po = boost::program_options;

    po::options_description desc("Usage: autordfxmi [-v] [-n namespacemap] [-o outfile] owlfile1 [owlfile2...]\n"
                                 "Processes an OWL file, and generates UML XMI file from it in current directory");

    desc.add_options()
            ("help,h", "Describe arguments")
            ("verbose,v", "Turn verbose output on.")
            ("namespacemap,n", po::value< std::vector<std::string> >(), "Adds supplementary namespaces prefix definition, in the form 'prefix:namespace IRI'. Defaults to empty.")
            ("outfile,o", po::value< std::string >(), "File to write to.")
            ("owlfile", po::value< std::vector<std::string> >(), "Input file (repeated)")
            ("preferredLang,l", po::value< std::vector<std::string> >(), "Preferred languages for documentation (repeated)")
            ("with_note", "UML with note");

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

    bool verbose = vm.count("verbose") > 0;
    bool withNote = vm.count("with_note") > 0;
    std::vector<std::string> preferredLang;
    if (vm.count("preferredLang")) {
        preferredLang = vm["preferredLang"].as< std::vector<std::string> >();
    }

    if(vm.count("namespacemap")) {
        for(const auto& prefix_namespace: vm["namespacemap"].as< std::vector<std::string> >()) {
            std::stringstream ss(prefix_namespace);
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

    std::ofstream ofs;
    std::ostream* out = &std::cout;
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
    std::string baseURI("http://");

    try {
        for ( const std::string& owlfile: vm["owlfile"].as< std::vector<std::string> >() ) {
            if ( verbose ) {
                std::cout << "Loading " << owlfile << " into model." << std::endl;
            }
            f.loadFromFile(owlfile, baseURI);
        }
    
        run(&f, verbose, withNote, preferredLang, out);
        return 0;

    } catch(const std::exception& e) {
        std::cerr << "Exception : " << e.what() << std::endl;
        return -1;
    }
}
