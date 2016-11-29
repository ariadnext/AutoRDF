ADD_EXECUTABLE(
    autordfcodegen
    main.cpp
    RdfsEntity.cpp
    DataProperty.cpp
    ObjectProperty.cpp
    Klass.cpp
    Utils.cpp
)

TARGET_LINK_LIBRARIES (
    autordfcodegen
    autordf-ontology
)

INSTALL(TARGETS autordfcodegen
    DESTINATION ${CMAKE_INSTALL_BINDIR})

if(${BUILD_TESTS} STREQUAL "1")
#FIXME
#ADD_SUBDIRECTORY(unittests)
endif()