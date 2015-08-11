INCLUDE_DIRECTORIES("${CMAKE_CURRENT_BINARY_DIR}")

SET_PROPERTY(SOURCE TestCodeGen.cpp APPEND PROPERTY OBJECT_DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf.h")
ADD_EXECUTABLE(
    testcodegen
    TestCodeGen.cpp
)
TARGET_LINK_LIBRARIES(
    testcodegen
    autordf
    boost_system
    boost_filesystem
)

ADD_CUSTOM_COMMAND(
    OUTPUT  "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf.h"
    COMMAND "${CMAKE_CURRENT_BINARY_DIR}/../autordfcodegen" "${CMAKE_CURRENT_SOURCE_DIR}/foaf_20140114.rdf"
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/../autordfcodegen"
)
