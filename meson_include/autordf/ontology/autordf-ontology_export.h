
#ifndef AUTORDF_ONTOLOGY_EXPORT_H
#define AUTORDF_ONTOLOGY_EXPORT_H

#ifdef AUTORDF_ONTOLOGY_STATIC_DEFINE
#  define AUTORDF_ONTOLOGY_EXPORT
#  define AUTORDF_ONTOLOGY_NO_EXPORT
#else
#  ifndef AUTORDF_ONTOLOGY_EXPORT
#    ifdef autordf_ontology_EXPORTS
        /* We are building this library */
#      define AUTORDF_ONTOLOGY_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define AUTORDF_ONTOLOGY_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef AUTORDF_ONTOLOGY_NO_EXPORT
#    define AUTORDF_ONTOLOGY_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef AUTORDF_ONTOLOGY_DEPRECATED
#  define AUTORDF_ONTOLOGY_DEPRECATED
#endif

#ifndef AUTORDF_ONTOLOGY_DEPRECATED_EXPORT
#  define AUTORDF_ONTOLOGY_DEPRECATED_EXPORT AUTORDF_ONTOLOGY_EXPORT AUTORDF_ONTOLOGY_DEPRECATED
#endif

#ifndef AUTORDF_ONTOLOGY_DEPRECATED_NO_EXPORT
#  define AUTORDF_ONTOLOGY_DEPRECATED_NO_EXPORT AUTORDF_ONTOLOGY_NO_EXPORT AUTORDF_ONTOLOGY_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef AUTORDF_ONTOLOGY_NO_DEPRECATED
#    define AUTORDF_ONTOLOGY_NO_DEPRECATED
#  endif
#endif

#endif /* AUTORDF_ONTOLOGY_EXPORT_H */
