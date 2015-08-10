#include "autordf/ResourceList.h"

#include <autordf/Factory.h>

namespace autordf {

void ResourceListIterator::sync() {
    if ( !_inSync ) {
        if ( _stmtit->subject.type == NodeType::RESOURCE ) {
            _current = std::make_shared<Resource>(_f->createIRIResource(_stmtit->subject.iri()));
        } else if ( _stmtit->subject.type == NodeType::BLANK ){
            _current = std::make_shared<Resource>(_f->createBlankResource(_stmtit->subject.bNodeId()));
        }
        _inSync = true;
    }
}

ResourceList::iterator ResourceList::_END(StatementList::iterator(nullptr), nullptr);
ResourceList::const_iterator ResourceList::_CEND(StatementList::const_iterator(nullptr), nullptr);

ResourceList::iterator ResourceList::begin() {
    StatementList statements = _f->find(_query);
    return ResourceList::iterator(statements.begin(), _f);
}

ResourceList::const_iterator ResourceList::begin() const {
    StatementList statements = _f->find(_query);
    return ResourceList::const_iterator(statements.begin(), _f);
}

}