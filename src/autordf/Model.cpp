#include "autordf/Model.h"

#include <errno.h>
#include <string.h>
#include <librdf.h>

#include <sstream>

#include "autordf/World.h"
#include "autordf/ModelPrivate.h"
#include "autordf/Storage.h"
#include "autordf/Parser.h"
#include "autordf/Stream.h"
#include "autordf/Uri.h"
#include "autordf/StatementConverter.h"

namespace autordf {

Model::Model() : _world(new World()), _model(new ModelPrivate(std::make_shared<Storage>())) {
}

void Model::loadFromFile(const std::string& path, const std::string& baseUri) {
    FILE *f = ::fopen(path.c_str(), "r");
    if ( !f ) {
        std::stringstream ss;
        ss << "Unable to open " << path << ": " << ::strerror(errno);
        throw std::runtime_error(ss.str().c_str());
    }

    try {
        std::shared_ptr<Parser> p = Parser::guessFromUri(Uri(path));
        if ( librdf_parser_parse_file_handle_into_model(p->get(), f, 0, Uri(baseUri).get(), _model->get()) ) {
            throw std::runtime_error("Failed to read model from file");
        }
    }
    catch(...) {
        ::fclose(f);
        throw;
    }
    ::fclose(f);
}

void Model::saveToFile(const std::string& path, const std::string& baseUri, const char *format) {
    if ( !format ) {
        format = librdf_parser_guess_name2(_world->get(), NULL, NULL, reinterpret_cast<const unsigned char *>(path.c_str()));
    }
    if ( !format ) {
        throw std::runtime_error("Unable to deduce format from file save name");
    }

    std::shared_ptr<librdf_serializer> s(librdf_new_serializer(_world->get(), format, 0, 0), librdf_free_serializer);
    if ( !s ) {
        throw std::runtime_error("Failed to construct RDF serializer");
    }

    if ( librdf_serializer_serialize_model_to_file(s.get(), path.c_str(), baseUri.length() ? Uri(baseUri).get() : nullptr, _model->get()) ) {
        throw std::runtime_error("Failed to export RDF model to file");
    }
}

StatementList Model::find(const Statement& req) {
    return StatementList(req, this);
}

void Model::add(const Statement &stmt) {
    std::shared_ptr<librdf_statement> librdfstmt(StatementConverter::toLibRdfStatement(stmt));
    if ( librdf_model_add_statement (_model->get(), librdfstmt.get()) ) {
        std::stringstream ss;
        ss << "Unable to add '" << stmt << "' statement";
        throw std::runtime_error(ss.str());
    }
}

void Model::remove(const Statement &stmt) {
    std::shared_ptr<librdf_statement> librdfstmt(StatementConverter::toLibRdfStatement(stmt));
    if ( librdf_model_remove_statement (_model->get(), librdfstmt.get()) ) {
        std::stringstream ss;
        ss << "Unable to remove '" << stmt << "' statement";
        throw std::runtime_error(ss.str());
    }
}
}