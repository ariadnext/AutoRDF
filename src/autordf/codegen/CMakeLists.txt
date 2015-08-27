ADD_EXECUTABLE(
    autordfcodegen
    main.cpp
    RdfsEndity.cpp
    Property.cpp
    DataProperty.cpp
    ObjectProperty.cpp
    Klass.cpp
    Utils.cpp
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