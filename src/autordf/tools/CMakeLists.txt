ADD_EXECUTABLE(
    autordfcodegen
    AutoRdfCodeGen.cpp
)

TARGET_LINK_LIBRARIES (
    autordfcodegen
    autordf
)

if(${BUILD_TESTS} STREQUAL "1")
    ADD_SUBDIRECTORY(unittests)
endif()