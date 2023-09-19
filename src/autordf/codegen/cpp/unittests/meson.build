foaf_src = custom_target(
  'foaf',
  input: 'foaf_20140114.ttl',
  output : ['AllInOne.cpp'],
  command : [autordf_codegen_exe, '-a', '-t', '@SOURCE_ROOT@/src/autordf/codegen/templates', '-l en', '-o', meson.current_build_dir(), '@INPUT@']
)

executable(
  'testcodegen',
  sources: foaf_src,
  include_directories: autordf_include_directories,
  link_with: autordf_lib,
  dependencies: sord_dep,
)
