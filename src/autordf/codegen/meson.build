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

if get_option('build_tests')
  subdir('cpp/unittests')
  subdir('python/unittests')
endif

#template_dir = join_paths(meson.current_source_dir(), 'templates')
#meson.override_find_program('autordfcodegen', autordfcodegen)
