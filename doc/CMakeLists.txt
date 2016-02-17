# add a target to generate API documentation with Doxygen
FIND_PACKAGE(Doxygen)
IF(DOXYGEN_FOUND)
    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    ADD_CUSTOM_TARGET(doc ALL
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen" VERBATIM
        )
    INSTALL(DIRECTORY
        ${CMAKE_CURRENT_BINARY_DIR}/html
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
    COMMAND "${CMAKE_BINARY_DIR}/src/autordf/codegen/autordfcodegen" -o geometry -a "${CMAKE_CURRENT_SOURCE_DIR}/geometry.owl"
    DEPENDS "${CMAKE_BINARY_DIR}/src/autordf/codegen/autordfcodegen" "${CMAKE_CURRENT_SOURCE_DIR}/geometry.owl"
)