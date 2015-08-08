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

namespace autordf {

Model::Model() : _world(new World()), _model(new ModelPrivate(std::make_shared<Storage>())) {
}

void Model::loadFromFile(const std::string& path) {
    FILE *f = ::fopen(path.c_str(), "r");
    if ( !f ) {
        std::stringstream ss;
        ss << "Unable to open " << path << ": " << ::strerror(errno);
        throw std::runtime_error(ss.str().c_str());
    }

    try {
        std::shared_ptr<Parser> p = Parser::guessFromUri(Uri(path));
        if ( librdf_parser_parse_file_handle_into_model(p->get(), f, 0, Uri("http://baseuri").get(), _model->get()) ) {
            throw std::runtime_error("Failed to read model from file");
        }
    }
    catch(...) {
        ::fclose(f);
        throw;
    }
    ::fclose(f);
}

StatementList Model::find() {
    std::shared_ptr<librdf_statement> search(librdf_new_statement(World().get()), librdf_free_statement);
    std::shared_ptr<Stream> stream(new Stream(librdf_model_find_statements(_model->get(), search.get())));
    if ( !stream ) {
        throw std::runtime_error("Redlang librdf_model_find_statements failed");
    }
    return autordf::StatementList(stream);
}
}