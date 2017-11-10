ADD_LIBRARY(
    autordf-ontology
    ${LIBRARY_TYPE}
    RdfsEntity.cpp
    Property.cpp
    Klass.cpp
    ObjectProperty.cpp
    DataProperty.cpp
    Ontology.cpp
    Validator.cpp
)
include(GenerateExportHeader)
generate_export_header(autordf-ontology)
INSTALL(FILES
	${CMAKE_CURRENT_BINARY_DIR}/autordf-ontology_export.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/autordf/ontology)

TARGET_LINK_LIBRARIES(
    autordf-ontology
    autordf
)
SET_TARGET_PROPERTIES(autordf-ontology PROPERTIES VERSION ${SOLIBVERSION})
IF(WINRT)
    SET_TARGET_PROPERTIES(autordf-ontology PROPERTIES VS_WINRT_COMPONENT TRUE)
ENDIF()

INSTALL(TARGETS autordf-ontology
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
