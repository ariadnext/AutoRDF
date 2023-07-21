autordf_codegen_src = [
  'main.cpp',
  'Environment.cpp',
  'CodeGenerator.cpp',
  'Klass.cpp',
]

autordf_codegen_cpp_src = [
  'cpp/CppCodeGenerator.cpp',
  'cpp/CppKlass.cpp',
]

autordf_codegen_python_src = [
  'python/PythonCodeGenerator.cpp',
  'python/PythonKlass.cpp',
]

autordf_codegen_exe = executable(
  meson.project_name() + 'codegen',
  sources: [autordf_codegen_src, autordf_codegen_cpp_src, autordf_codegen_python_src],
  include_directories: [autordf_include_directories, 'vendors'],
  link_with: [autordf_lib, autordf_ontology_lib],
  dependencies: [boost_dep],
  install: true,
)

# Templates installation
install_subdir(
  'templates',
  install_dir: get_option('datadir') / meson.project_name(),
)

if get_option('build_tests')
  subdir('cpp/unittests')
  subdir('python/unittests')
endif

# Helps finding autordfcodegen in other projects without installing it in the PATH
meson.override_find_program('autordfcodegen', autordf_codegen_exe)
