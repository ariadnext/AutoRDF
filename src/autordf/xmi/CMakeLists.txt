ADD_EXECUTABLE(
    autordfxmi
    main.cpp
)

TARGET_LINK_LIBRARIES (
        autordfxmi
    autordf-ontology
    ${Boost_LIBRARIES}
)

INSTALL(TARGETS autordfxmi
    DESTINATION ${CMAKE_INSTALL_BINDIR})
