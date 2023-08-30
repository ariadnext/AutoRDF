ADD_EXECUTABLE(
    autordfxmi
    main_xmi.cpp
)

TARGET_LINK_LIBRARIES (
    autordfxmi
    autordf-ontology
    ${Boost_LIBRARIES}
)

INSTALL(TARGETS autordfxmi
    DESTINATION ${CMAKE_INSTALL_BINDIR})

ADD_EXECUTABLE(
    autordfpuml
    main_puml.cpp
)

TARGET_LINK_LIBRARIES (
    autordfpuml
    autordf-ontology
    ${Boost_LIBRARIES}
)

INSTALL(TARGETS autordfpuml
    DESTINATION ${CMAKE_INSTALL_BINDIR})