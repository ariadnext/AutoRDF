#include "CppCodeGenerator.h"

#include "../Environment.h"

#include "CppKlass.h"

namespace autordf {
namespace codegen {
namespace cpp {
void CppCodeGenerator::runInternal(const ontology::Ontology& ontology, inja::Environment& renderer) {
    std::set<std::string> cppNameSpaces;

    renderer.add_callback("cppType", 1, [](inja::Arguments& args) {
        return rdfTypeToCppType(args[0]->get<int>());
    });

    renderer.add_callback("cppManyType", 1, [](inja::Arguments& args) {
        auto type = args[0]->get<int>();
        auto manyType = rdfTypeToCppManyType(type);

        if (manyType.empty()) {
            return "std::vector<" + rdfTypeToCppType(type) + ">";
        } else {
            return manyType;
        }
    });

    if (Environment::verbose) {
        std::cout << "Starting C++ code generation" << std::endl;
    }

    for (auto const& klassMapItem: ontology.classUri2Ptr()) {
        auto klass = CppKlass(*klassMapItem.second, renderer);
        klass.buildTemplateData();

        // created directory if needed
        Environment::createOutDirectory(klass.packagePath());
        cppNameSpaces.insert(klass.basePackageName());

        if (Environment::verbose) {
            std::cout << "Generating class '" << klass.className() << "'..." << std::endl;
        }
        klass.generate();
        if (Environment::verbose) {
            std::cout << "Generation done." << std::endl;
        }
    }

    // Generate all inclusions files
    if (Environment::verbose) {
        std::cout << "Generating all inclusions files..." << std::endl;
    }
    for (const std::string& cppNameSpace : cppNameSpaces) {
        std::ofstream out;
        if (!Environment::namespace_.empty()) {
            Environment::createFile(Environment::namespace_ + "/" +cppNameSpace + "/" + cppNameSpace + ".h", out);
        } else {
            Environment::createFile(cppNameSpace + "/" + cppNameSpace + ".h", out);
        }

        nlohmann::json data;

        if (Environment::verbose) {
            std::cout << "For namespace '" << cppNameSpace << "'" << std::endl;
        }

        data["namespace"] = cppNameSpace;
        data["classes"] = nlohmann::json::array();

        for (auto const& klassMapItem: ontology.classUri2Ptr()) {
            auto klass = CppKlass(*klassMapItem.second, renderer);
            klass.buildTemplateData(false);
            if (klass.basePackageName() == cppNameSpace) {
                data["classes"].push_back(klass.templateData());
            }
        }

        if (Environment::verbose) {
            std::cout << "Rendering template 'cpp/all_inclusions.tpl' to '" << Environment::outdir << "/" << cppNameSpace << "/" << cppNameSpace << ".h'...";
        }
        auto tpl = renderer.parse_template("cpp/all_inclusions.tpl");
        renderer.render_to(out, tpl, data);
        out.close();
        if (Environment::verbose) {
            std::cout << " Done." << std::endl;
        }
    }
    if (Environment::verbose) {
        std::cout << "Generation done." << std::endl;
    }

    // Generate all in one cpp file
    if (_allInOne) {
        if (Environment::verbose) {
            std::cout << "Generating all-in-one cpp file..." << std::endl;
        }

        std::ofstream out;
        if (!Environment::namespace_.empty()) {
            Environment::createFile("/" + Environment::namespace_ +"/AllInOne.cpp", out);
        } else {
            Environment::createFile("/AllInOne.cpp", out);
        }

        nlohmann::json data;
        data["classes"] = nlohmann::json::array();

        for (auto const& klassMapItem: ontology.classUri2Ptr()) {
            auto klass = CppKlass(*klassMapItem.second, renderer);
            klass.buildTemplateData(false);
            data["classes"].push_back(klass.templateData());
        }

        if (Environment::verbose) {
            std::cout << "Rendering template 'cpp/all_in_one.tpl' to '" << Environment::outdir << "/AllInOne.cpp'...";
        }
        auto tpl = renderer.parse_template("cpp/all_in_one.tpl");
        renderer.render_to(out, tpl, data);
        out.close();
        if (Environment::verbose) {
            std::cout << " Done." << std::endl;
        }

        if (Environment::verbose) {
            std::cout << "Generation done." << std::endl;
        }
    }
}

std::string CppCodeGenerator::rdfTypeToCppType(int type) {
    if (type == -1) {
        return "autordf::PropertyValue";
    }

    auto rdfType = cvt::RdfTypeEnum(type);
    switch (rdfType) {
        case cvt::RdfTypeEnum::xsd_string:
            return "std::string";
        case cvt::RdfTypeEnum::xsd_boolean:
            return "bool";
        case cvt::RdfTypeEnum::xsd_decimal:
            return "double";
        case cvt::RdfTypeEnum::xsd_float:
            return "float";
        case cvt::RdfTypeEnum::xsd_double:
            return "double";
        case cvt::RdfTypeEnum::xsd_dateTime:
            return "boost::posix_time::ptime";
        case cvt::RdfTypeEnum::xsd_integer:
            return "long long int";
        case cvt::RdfTypeEnum::xsd_dateTimeStamp:
            return "boost::posix_time::ptime";
        case cvt::RdfTypeEnum::xsd_nonNegativeInteger:
        case cvt::RdfTypeEnum::xsd_positiveInteger:
            return "long long unsigned int";
        case cvt::RdfTypeEnum::xsd_nonPositiveInteger:
        case cvt::RdfTypeEnum::xsd_negativeInteger:
        case cvt::RdfTypeEnum::xsd_long:
            return "long long int";
        case cvt::RdfTypeEnum::xsd_unsignedLong:
            return "unsigned long long int";
        case cvt::RdfTypeEnum::xsd_int:
            return "long int";
        case cvt::RdfTypeEnum::xsd_unsignedInt:
            return "long unsigned int";
        case cvt::RdfTypeEnum::xsd_short:
            return "short";
        case cvt::RdfTypeEnum::xsd_unsignedShort:
            return "unsigned short";
        case cvt::RdfTypeEnum::xsd_byte:
            return "char";
        case cvt::RdfTypeEnum::xsd_unsignedByte:
            return "unsigned char";
        case cvt::RdfTypeEnum::rdf_langString:
            return "autordf::I18String";
    }
}

std::string CppCodeGenerator::rdfTypeToCppManyType(int type) {
    if (type == -1) {
        return "";
    }

    auto rdfType = cvt::RdfTypeEnum(type);
    switch (rdfType) {
        case cvt::RdfTypeEnum::xsd_string:
        case cvt::RdfTypeEnum::xsd_boolean:
        case cvt::RdfTypeEnum::xsd_decimal:
        case cvt::RdfTypeEnum::xsd_float:
        case cvt::RdfTypeEnum::xsd_double:
        case cvt::RdfTypeEnum::xsd_dateTime:
        case cvt::RdfTypeEnum::xsd_integer:
        case cvt::RdfTypeEnum::xsd_dateTimeStamp:
        case cvt::RdfTypeEnum::xsd_nonNegativeInteger:
        case cvt::RdfTypeEnum::xsd_positiveInteger:
        case cvt::RdfTypeEnum::xsd_nonPositiveInteger:
        case cvt::RdfTypeEnum::xsd_negativeInteger:
        case cvt::RdfTypeEnum::xsd_long:
        case cvt::RdfTypeEnum::xsd_unsignedLong:
        case cvt::RdfTypeEnum::xsd_int:
        case cvt::RdfTypeEnum::xsd_unsignedInt:
        case cvt::RdfTypeEnum::xsd_short:
        case cvt::RdfTypeEnum::xsd_unsignedShort:
        case cvt::RdfTypeEnum::xsd_byte:
        case cvt::RdfTypeEnum::xsd_unsignedByte:
            return "";
        case cvt::RdfTypeEnum::rdf_langString:
            return "autordf::I18StringVector";
    }
}
}
}
}
