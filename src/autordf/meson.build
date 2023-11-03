autordf_src = [
  'Model.cpp',
  'NodeType.cpp',
  'Node.cpp',
  'NodeList.cpp',
  'Statement.cpp',
  'StatementList.cpp',
  'Resource.cpp',
  'Property.cpp',
  'PropertyValue.cpp',
  'Factory.cpp',
  'Object.cpp',
  'Exception.cpp',
  'Uri.cpp',
  'Storage.cpp',
  'I18String.cpp',
  'I18StringVector.cpp',
]

internal_src_folder = 'internal'
autordf_internal_src = [
  internal_src_folder / 'World.cpp',
  internal_src_folder / 'ModelPrivate.cpp',
  internal_src_folder / 'Parser.cpp',
  internal_src_folder / 'Uri.cpp',
  internal_src_folder / 'Stream.cpp',
  internal_src_folder / 'StatementConverter.cpp',
]

cvt_src_folder = 'cvt'
autordf_cvt_src = [
  cvt_src_folder / 'RdfTypeEnum.cpp',
]

notification_src_folder = 'notification'
autordf_notification_src = [
  notification_src_folder / 'ANotifier.cpp',
  notification_src_folder / 'NotifierLocker.cpp',
  notification_src_folder / 'DefaultNotifier.cpp'
]

autordf_lib = library(
  meson.project_name(),
  sources: [autordf_src, autordf_internal_src, autordf_cvt_src, autordf_notification_src],
  include_directories: [autordf_include_directories, autordf_private_include_directories],
  dependencies: [boost_dep, sord_dep],
  install: true,
)

autordf_dep = declare_dependency(
  link_with: autordf_lib,
  compile_args: '-DUSE_SORD',
  include_directories: autordf_include_directories,
  dependencies: [boost_dep, sord_dep],
  variables: {'templates_path': meson.current_source_dir() / 'codegen' / 'templates'},
)

# Pkgconfig file installation
pkgconfig.generate(
  autordf_lib,
  libraries: sord_dep,
  extra_cflags: '-DUSE_SORD',
)

subdir('ontology')
if get_option('build_tools')
  subdir('codegen')
  subdir('uml')
endif
if get_option('build_python_binding')
  subdir('pybind')
endif
