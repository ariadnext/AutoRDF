find_package(Boost COMPONENTS date_time)

ADD_LIBRARY(
    autordf
    ${LIBRARY_TYPE}
    World.cpp
    Model.cpp
    ModelPrivate.cpp
    Storage.cpp
    Parser.cpp
    Uri.cpp
    Stream.cpp
    Node.cpp
    NodeType.cpp
    Statement.cpp
    StatementList.cpp
    StatementConverter.cpp
    Resource.cpp
    Property.cpp
    Factory.cpp
    Object.cpp
    Exception.cpp
)
TARGET_LINK_LIBRARIES(
    autordf
    ${REDLAND_LIBRARIES}
    boost_date_time
)
SET_TARGET_PROPERTIES(autordf PROPERTIES VERSION ${SOLIBVERSION})

INSTALL(TARGETS autordf
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

INSTALL(DIRECTORY
    ${CMAKE_SOURCE_DIR}/include/${PROJECT_NAME}

    DESTINATION include/
)

ADD_SUBDIRECTORY(codegen)
ADD_SUBDIRECTORY(ontology)