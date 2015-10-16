ADD_LIBRARY(
    autordf-ontology
    ${LIBRARY_TYPE}
    RdfsEndity.cpp
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

INSTALL(FILES
    ../../include/autordf/ontology/DataProperty.h
    ../../include/autordf/ontology/Klass.h
    ../../include/autordf/ontology/ObjectProperty.h
    ../../include/autordf/ontology/Ontology.h
    ../../include/autordf/ontology/Property.h
    ../../include/autordf/ontology/RdfsEntity.h
    DESTINATION include/autordf
)
