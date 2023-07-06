autordfcodegen_includes = include_directories(['.', 'cpp', 'vendors', 'python', '../../../include', '../../../meson_include'])

boost_dep = dependency('boost', modules : ['program_options'])

source_files = ['main.cpp', 'Environment.cpp', 'CodeGenerator.cpp', 'Klass.cpp', 'cpp/CppCodeGenerator.cpp', 'cpp/CppKlass.cpp', 'python/PythonCodeGenerator.cpp', 'python/PythonKlass.cpp']

autordfcodegen = executable(
    'autordfcodegen',
    sources: source_files,
    include_directories: autordfcodegen_includes,
    link_with: [autordf_library, autordf_ontology_lib],
    dependencies: [boost_dep],
    install: true
)

template_dir = join_paths(meson.current_source_dir(), 'templates')

meson.override_find_program('autordfcodegen', autordfcodegen)
