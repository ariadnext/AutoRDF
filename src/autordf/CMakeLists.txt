SET(REQUIRED_BOOST_COMPONENTS date_time program_options)
IF(APPLE AND NOT NATIVE)
set(Boost_INCLUDE_DIRS ${CMAKE_FRAMEWORK_EXTRA_PATH}/boost.framework)
set(Boost_LIBRARIES boost)
message(STATUS "FORCED Boost_INCLUDE_DIRS to ${Boost_INCLUDE_DIRS}")
ELSEIF(WINRT)
find_package(Boost REQUIRED COMPONENTS ${REQUIRED_BOOST_COMPONENTS})
ELSE(APPLE AND NOT NATIVE)
find_package(Boost REQUIRED COMPONENTS ${REQUIRED_BOOST_COMPONENTS})
ENDIF(APPLE AND NOT NATIVE)
IF(WIN32)
    MESSAGE("BOOST: ${BOOST_INCLUDE_DIRS} Boost: ${Boost_INCLUDE_DIRS}, ${Boost_LIBRARY_DIRS}, ${Boost_LIBRARIES}")
    IF("${LIBRARY_TYPE}" STREQUAL "SHARED")
        LIST(APPEND Boost_LIBRARIES Boost::dynamic_linking)
        LIST(APPEND Boost_DATE_TIME_LIBRARY Boost::dynamic_linking)
        LIST(APPEND Boost_PROGRAM_OPTIONS_LIBRARY Boost::dynamic_linking)
    ELSE()
        LIST(APPEND Boost_LIBRARIES Boost::disable_autolinking)
        LIST(APPEND Boost_DATE_TIME_LIBRARY Boost::disable_autolinking)
        LIST(APPEND Boost_PROGRAM_OPTIONS_LIBRARY Boost::disable_autolinking)
    ENDIF()
ENDIF()
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

ADD_LIBRARY(
    autordf
    ${LIBRARY_TYPE}
    Model.cpp
    NodeType.cpp
    Node.cpp
    NodeList.cpp
    Statement.cpp
    StatementList.cpp
    Resource.cpp
    Property.cpp
    PropertyValue.cpp
    Factory.cpp
    Object.cpp
    Exception.cpp
    Uri.cpp
    Storage.cpp
    internal/World.cpp
    internal/ModelPrivate.cpp
    internal/Parser.cpp
    internal/Uri.cpp
    internal/Stream.cpp
    internal/StatementConverter.cpp
    cvt/RdfTypeEnum.cpp
    I18String.cpp
    I18StringVector.cpp
    notification/ANotifier.cpp
    notification/NotifierLocker.cpp
    notification/DefaultNotifier.cpp
)

include(GenerateExportHeader)
generate_export_header(autordf)
INSTALL(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/autordf_export.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/autordf)

TARGET_LINK_LIBRARIES(
    autordf
    ${RDF_C_API_LIBRARIES}
    ${Boost_DATE_TIME_LIBRARY}
)
SET_TARGET_PROPERTIES(autordf PROPERTIES VERSION ${SOLIBVERSION})
IF(WINRT)
    SET_TARGET_PROPERTIES(autordf PROPERTIES VS_WINRT_COMPONENT TRUE)
ENDIF()

IF(GENERATE_PYTHON_BINDINGS)
    find_package(pybind11 REQUIRED)
    FILE(GLOB AUTORDF_PY_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/pybind/*.cpp)
    pybind11_add_module(autordf_py ${AUTORDF_PY_SOURCES})
    target_link_libraries(autordf_py autordf)

    INSTALL(TARGETS autordf_py DESTINATION ${PYTHON_SITE_PACKAGE})
    IF(${BUILD_TESTS} STREQUAL "1")
        FILE(GLOB PYBIND_TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/pybind/unittests/*.py
            ${CMAKE_CURRENT_SOURCE_DIR}/pybind/unittests/*.ttl)
        message("${PYBIND_TEST_SOURCES}")
        INSTALL(FILES ${PYBIND_TEST_SOURCES} DESTINATION ${CMAKE_BINARY_DIR}/tests/)
    ENDIF()
ENDIF()

INSTALL(TARGETS autordf DESTINATION ${CMAKE_INSTALL_LIBDIR})
ADD_SUBDIRECTORY(ontology)

if(${BUILD_TOOLS} STREQUAL "1")
ADD_SUBDIRECTORY(codegen)
ADD_SUBDIRECTORY(uml)
endif()
