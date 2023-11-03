project('autordf', 'cpp', version:'1.0.0', default_options : ['cpp_std=c++20'])

add_project_arguments('-DUSE_SORD', language : 'cpp')

pkgconfig = import('pkgconfig')

boost_dep = dependency('boost', modules: ['program_options', 'date_time', 'filesystem'])
# Fallback is used when sord is in a subproject
sord_dep = dependency('sord', fallback: ['sord', 'sord_dep'])

autordf_include_directories = include_directories(['include', 'meson_include'])
autordf_private_include_directories = include_directories(['src', 'src/autordf'])

# Headers installation
include_folder = 'include' / 'autordf'
install_headers(
  include_folder / 'cAPI.h',
  include_folder / 'Exception.h',
  include_folder / 'Factory.h',
  include_folder / 'I18String.h',
  include_folder / 'I18StringVector.h',
  include_folder / 'Model.h',
  include_folder / 'Node.h',
  include_folder / 'NodeList.h',
  include_folder / 'NodeType.h',
  include_folder / 'Object.h',
  include_folder / 'Property.h',
  include_folder / 'PropertyValue.h',
  include_folder / 'Resource.h',
  include_folder / 'Statement.h',
  include_folder / 'StatementList.h',
  include_folder / 'Storage.h',
  include_folder / 'Uri.h',
  'meson_include' / 'autordf' / 'autordf_export.h',
  subdir: 'autordf',
)

cvt_include_folder = 'include' / 'autordf' / 'cvt'
install_headers(
  cvt_include_folder / 'Cvt.h',
  cvt_include_folder / 'RdfTypeEnum.h',
  subdir: 'autordf/cvt',
)

ontology_include_folder = 'include' / 'autordf' / 'ontology'
install_headers(
  ontology_include_folder / 'AnnotationProperty.h',
  ontology_include_folder / 'DataProperty.h',
  ontology_include_folder / 'Klass.h',
  ontology_include_folder / 'ObjectProperty.h',
  ontology_include_folder / 'Ontology.h',
  ontology_include_folder / 'Property.h',
  ontology_include_folder / 'RdfsEntity.h',
  ontology_include_folder / 'Validator.h',
  'meson_include' / 'autordf' / 'ontology' / 'autordf-ontology_export.h',
  subdir: 'autordf/ontology',
)

notification_include_folder = 'include' / 'autordf' / 'notification'
install_headers(
  notification_include_folder / 'ANotifier.h',
  subdir: 'autordf/notification'
)

subdir('src/autordf')
if get_option('build_tests')
  subdir('tests')
endif
if get_option('build_doc')
  subdir('doc')
endif
