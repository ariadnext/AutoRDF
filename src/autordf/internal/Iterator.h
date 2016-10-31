#ifndef AUTORDF_ITERATOR_H
#define AUTORDF_ITERATOR_H

#ifdef LIBRDF_IN_SUBDIRS
#include <librdf/librdf.h>
#else
#include <librdf.h>
#endif

namespace autordf {
namespace internal {

class Iterator {
public:
    /**
     * Constructor takes ownership of the iterator object
     */
    Iterator(librdf_iterator* iterator) : _iterator(iterator) {}

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
    void* object() const { return librdf_iterator_get_object(_iterator);  }

    librdf_iterator* get() const { return _iterator; }

private:
    librdf_iterator* _iterator;
};

}
}

#endif //AUTORDF_ITERATOR_H
