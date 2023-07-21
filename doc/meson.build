doxygen = find_program('doxygen', required: false)
if doxygen.found()
  doxygen_conf_data = configuration_data()
  doxygen_conf_data.set('PROJECT_SOURCE_DIR', meson.project_source_root())
  doxygen_conf_data.set('PROJECT_BUILD_DIR', meson.current_build_dir())
  doxygen_conf = configure_file(
    input: 'Doxyfile.in',
    output: 'Doxyfile',
    configuration: doxygen_conf_data,
    install: false,
  )

  custom_target(
    'doc',
    input: doxygen_conf,
    output: 'autordf',
    command: [doxygen, '@INPUT@'],
    build_by_default: true,
    install: true,
    install_dir: join_paths(get_option('datadir'), 'doc'),
  )
endif
