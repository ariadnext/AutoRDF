gtest_dep = dependency('gtest')

unittests_folder = '..' / 'src' / 'autordf' / 'unittests'
unittests_src = [
  unittests_folder / '01_ModelUnitTests.cpp',
  unittests_folder / '02_LoadSaveUnitTests.cpp',
  unittests_folder / '03_ObjectlUnitTests.cpp',
  unittests_folder / '04_cvtUnitTests.cpp',
  unittests_folder / '05_PropertyValueUnitTests.cpp',
]

ontology_unittests_folder = '..' / 'src' / 'autordf' / 'ontology' / 'unittests'
ontology_unittests_src = [
  ontology_unittests_folder / '01_ValidatorUnitTests.cpp',
]

unittests_exe = executable(
  'gtest-all',
  sources: ['gtest-all.cpp', unittests_src],
  include_directories: autordf_include_directories,
  link_with: [autordf_lib, autordf_ontology_lib],
  dependencies: [gtest_dep, boost_dep],
)

custom_target(
  'check',
  output: 'unittests.xml',
  command: [unittests_exe, '--gtest_output="xml:@OUTPUT@"']
)

executable(
  'perftest',
  sources: 'perftest.cpp',
  include_directories: autordf_include_directories,
  link_with: autordf_lib,
)
