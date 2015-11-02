ADD_EXECUTABLE(
    autordfxmi
    main.cpp
)

TARGET_LINK_LIBRARIES (
        autordfxmi
    autordf-ontology
)

INSTALL(TARGETS autordfxmi
    DESTINATION ${CMAKE_INSTALL_BINDIR})
