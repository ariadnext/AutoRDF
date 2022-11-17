ADD_EXECUTABLE(
    autordfcodegen
    main.cpp
    Environment.cpp
    CodeGenerator.cpp
    Klass.cpp
    cpp/CppCodeGenerator.cpp
    cpp/CppKlass.cpp
    python/PythonCodeGenerator.cpp
    python/PythonKlass.cpp
)

FILE(GLOB_RECURSE AUTORDF_TEMPLATES ${CMAKE_CURRENT_SOURCE_DIR}/templates/*.tpl)

TARGET_LINK_LIBRARIES (
    autordfcodegen
    autordf-ontology
    ${Boost_LIBRARIES}
)

TARGET_INCLUDE_DIRECTORIES(
    autordfcodegen
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/vendors
)

INSTALL(TARGETS autordfcodegen
    DESTINATION ${CMAKE_INSTALL_BINDIR})

INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/templates DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME})

if(${BUILD_TESTS} STREQUAL "1")
    ADD_SUBDIRECTORY(cpp/unittests)
    ADD_SUBDIRECTORY(python/unittests)
endif()
