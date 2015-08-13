ADD_EXECUTABLE(
    autordfcodegen
    AutoRdfCodeGen.cpp
)

TARGET_LINK_LIBRARIES (
    autordfcodegen
    autordf
)

INSTALL(TARGETS autordfcodegen
    DESTINATION ${CMAKE_INSTALL_BINDIR})

if(${BUILD_TESTS} STREQUAL "1")
    ADD_SUBDIRECTORY(unittests)
endif()