#ifndef AUTORDF_ITERATOR_H
#define AUTORDF_ITERATOR_H

#include <autordf/internal/cAPI.h>

namespace autordf {
namespace internal {

#if defined(USE_REDLAND)
class Iterator {
public:
    /**
     * Constructor takes ownership of the iterator object
     */
    Iterator(c_api_iterator* iterator) : _iterator(iterator) {}

    ~Iterator() {
        if (_iterator) {
            librdf_free_iterator(_iterator);
            _iterator = nullptr;
        }
    }

    /** Returns false if no more objects */
    bool next() { return _iterator ? !librdf_iterator_next(_iterator) : false; }

    /** Returns true if at end */
    bool end() { return _iterator ? librdf_iterator_end(_iterator) : true; }

    /** Returns pointed object */
    c_api_node* object() const { return static_cast<c_api_node*>(librdf_iterator_get_object(_iterator));  }

    c_api_iterator* get() const { return _iterator; }

private:
    c_api_iterator* _iterator;
};
#elif defined(USE_SORD)
class Iterator {
public:
    /**
     * Constructor takes ownership of the iterator object
     */
    Iterator(c_api_iterator* iterator, SordQuadIndex index) : _iterator(iterator), _index(index) {}

    ~Iterator() {
        if (_iterator) {
            sord_iter_free(_iterator);
            _iterator = nullptr;
        }
    }

    /** Returns false if no more objects */
    bool next() { return _iterator ? !sord_iter_next(_iterator) : false; }

    /** Returns true if at end */
    bool end() { return _iterator ? sord_iter_end(_iterator) : true; }

    /** Returns pointed object */
    c_api_node* object() const { return const_cast<SordNode*>(sord_iter_get_node(_iterator, _index));  }

    c_api_iterator* get() const { return _iterator; }

private:
    c_api_iterator *_iterator;
    SordQuadIndex _index;
};
#endif

}
}

#endif //AUTORDF_ITERATOR_H
