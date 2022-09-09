ADD_EXECUTABLE(
    autordfcodegen-python
    main.cpp
    RdfsEntityPython.cpp
    DataPropertyPython.cpp
    ObjectPropertyPython.cpp
    KlassPython.cpp
    UtilsPython.cpp
    ../UtilsCommon.cpp
)

TARGET_LINK_LIBRARIES (
    autordfcodegen-python
    autordf-ontology
    ${Boost_LIBRARIES}
)

INSTALL(TARGETS autordfcodegen-python
    DESTINATION ${CMAKE_INSTALL_BINDIR})

if(${BUILD_TESTS} STREQUAL "1")
    ADD_SUBDIRECTORY(unittests)
endif()