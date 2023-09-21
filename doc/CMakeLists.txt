# add a target to generate API documentation with Doxygen
FIND_PACKAGE(Doxygen)
IF(DOXYGEN_FOUND)
    set(PROJECT_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})
    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    ADD_CUSTOM_TARGET(doc ALL
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen" VERBATIM
        )
    INSTALL(DIRECTORY
        ${CMAKE_CURRENT_BINARY_DIR}/autordf/html
        DESTINATION ${CMAKE_INSTALL_DOCDIR}
        )
ENDIF(DOXYGEN_FOUND)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

ADD_EXECUTABLE(
    example
    example.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/geometry/AllInOne.cpp
)

TARGET_LINK_LIBRARIES (
    example
    autordf
)

ADD_CUSTOM_COMMAND(
    OUTPUT
    ${CMAKE_CURRENT_BINARY_DIR}/geometry/AllInOne.cpp
    COMMAND "${CMAKE_BINARY_DIR}/src/autordf/codegen/autordfcodegen" -a -t "${CMAKE_SOURCE_DIR}/src/autordf/codegen/templates" -s geometry "${CMAKE_CURRENT_SOURCE_DIR}/geometry.ttl"
    COMMAND "${CMAKE_BINARY_DIR}/src/autordf/uml/autordfxmi" -o geometry.xmi "${CMAKE_CURRENT_SOURCE_DIR}/geometry.ttl"
    DEPENDS "${CMAKE_BINARY_DIR}/src/autordf/codegen/autordfcodegen" "${CMAKE_BINARY_DIR}/src/autordf/uml/autordfxmi" "${CMAKE_CURRENT_SOURCE_DIR}/geometry.ttl"
    COMMENT "Generating C++ classes and UML model for example geometry ontology"
)
