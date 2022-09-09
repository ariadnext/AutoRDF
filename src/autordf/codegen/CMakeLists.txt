ADD_EXECUTABLE(
    autordfcodegen
    main.cpp
    RdfsEntity.cpp
    DataProperty.cpp
    ObjectProperty.cpp
    Klass.cpp
    Utils.cpp
    UtilsCommon.cpp
)

TARGET_LINK_LIBRARIES (
    autordfcodegen
    autordf-ontology
    ${Boost_LIBRARIES}
)

INSTALL(TARGETS autordfcodegen
    DESTINATION ${CMAKE_INSTALL_BINDIR})

if(${BUILD_TESTS} STREQUAL "1")
    ADD_SUBDIRECTORY(unittests)
endif()