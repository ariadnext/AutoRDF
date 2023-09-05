project('autordf', 'cpp', version:'1.0.0', default_options : ['cpp_std=c++20'])

# Standard autordf library
autordf_library_includes = include_directories(['include', 'src', 'src/autordf', 'meson_include'])
boost_dep = dependency('boost')

sord = subproject('sord')
sord_dep = sord.get_variable('sord_dep')

source_files = ['src/autordf/Model.cpp', 'src/autordf/NodeType.cpp','src/autordf/Node.cpp','src/autordf/NodeList.cpp','src/autordf/Statement.cpp','src/autordf/StatementList.cpp','src/autordf/Resource.cpp','src/autordf/Property.cpp','src/autordf/PropertyValue.cpp','src/autordf/Factory.cpp','src/autordf/Object.cpp','src/autordf/Exception.cpp','src/autordf/Uri.cpp','src/autordf/Storage.cpp','src/autordf/internal/World.cpp','src/autordf/internal/ModelPrivate.cpp','src/autordf/internal/Parser.cpp','src/autordf/internal/Uri.cpp','src/autordf/internal/Stream.cpp','src/autordf/internal/StatementConverter.cpp','src/autordf/cvt/RdfTypeEnum.cpp','src/autordf/I18String.cpp','src/autordf/I18StringVector.cpp']

autordf_library = library(
    meson.project_name(),
    sources: source_files,
    dependencies: [boost_dep, sord_dep],
    include_directories: autordf_library_includes,
    install: true
)

subdir('src/autordf/ontology')
subdir('src/autordf/codegen')

autordf_dep = declare_dependency(
    link_with: [autordf_library],
    include_directories: autordf_library_includes
)
