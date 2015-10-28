ADD_LIBRARY(
    autordf-ontology
    ${LIBRARY_TYPE}
    RdfsEntity.cpp
    Property.cpp
    Klass.cpp
    ObjectProperty.cpp
    DataProperty.cpp
    Ontology.cpp
)
TARGET_LINK_LIBRARIES(
    autordf-ontology
    autordf
)
SET_TARGET_PROPERTIES(autordf-ontology PROPERTIES VERSION ${SOLIBVERSION})

INSTALL(TARGETS autordf-ontology
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
