#ifndef AUTORDF_INTERNAL_CAPI_H
#define AUTORDF_INTERNAL_CAPI_H

#if defined(USE_REDLAND)
    #ifdef LIBRDF_IN_SUBDIRS
    #include <librdf/librdf.h>
    #include <raptor2/raptor2.h>
    #else
    #include <librdf.h>
    #include <raptor2.h>
    #endif
    typedef librdf_node      c_api_node;
    typedef librdf_statement c_api_statement;
    typedef librdf_model     c_api_model;
    typedef librdf_world     c_api_world;
    typedef librdf_iterator  c_api_iterator;
    typedef librdf_stream    c_api_stream;

    extern "C" unsigned char* librdf_world_get_genid(librdf_world* world);
#elif defined(USE_SORD)
    #include <sord/sord.h>
    #include <serd/serd.h>
    typedef SordNode  c_api_node;
    typedef SordQuad  c_api_statement;
    typedef SordModel c_api_model;
    typedef SordWorld c_api_world;
    typedef SordIter  c_api_iterator;
    typedef SordIter  c_api_stream;
#endif

#endif // AUTORDF_INTERNAL_CAPI_H
