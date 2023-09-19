python = import('python')
python_inst = python.find_installation('python3')
python_dep = python_inst.dependency()

autordf_py_src = [
  'FactoryPybind.cpp',
  'I18StringPybind.cpp',
  'ModelPybind.cpp',
  'ObjectPybind.cpp',
  'PropertyValuePybind.cpp',
  'PybindModule.cpp',
  'UriPybind.cpp',
]

autordf_py_lib = python_inst.extension_module(
  'autordf_py',
  sources: autordf_py_src,
  include_directories: autordf_include_directories,
  dependencies : [python_dep],
  link_language : 'cpp',
  override_options: ['cpp_rtti=true'],
)
