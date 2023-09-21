autordf_ontology_src = [
  'RdfsEntity.cpp',
  'Property.cpp',
  'Klass.cpp',
  'ObjectProperty.cpp',
  'DataProperty.cpp',
  'AnnotationProperty.cpp',
  'Ontology.cpp',
  'Validator.cpp',
]

autordf_ontology_lib = library(
  meson.project_name() + '-ontology',
  sources: autordf_ontology_src,
  include_directories: autordf_include_directories,
  link_with: autordf_lib,
  install: true,
)

autordf_ontology_dep = declare_dependency(
  link_with: [autordf_lib, autordf_ontology_lib],
  include_directories: autordf_include_directories,
)

# Pkgconfig file installation
pkgconfig.generate(
  autordf_ontology_lib,
  libraries: autordf_lib,
)
