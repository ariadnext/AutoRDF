autordf_ontology_includes = ['../../../include', '../../../meson_include']

source_files = ['RdfsEntity.cpp', 'Property.cpp', 'Klass.cpp', 'ObjectProperty.cpp', 'DataProperty.cpp', 'AnnotationProperty.cpp', 'Ontology.cpp', 'Validator.cpp']

autordf_ontology_lib = library(
    'autordf-ontology',
    sources: source_files,
    include_directories: autordf_ontology_includes,
    link_with: autordf_library,
    install: true
)
