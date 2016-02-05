find_package(Boost COMPONENTS date_time)

ADD_LIBRARY(
    autordf
    ${LIBRARY_TYPE}
    Model.cpp
    NodeType.cpp
    Node.cpp
    NodeList.cpp
    Statement.cpp
    StatementList.cpp
    Resource.cpp
    Property.cpp
    PropertyValue.cpp
    PropertyValueVector.cpp
    Factory.cpp
    Object.cpp
    Exception.cpp
    Uri.cpp
    internal/World.cpp
    internal/ModelPrivate.cpp
    internal/Storage.cpp
    internal/Parser.cpp
    internal/Uri.cpp
    internal/Stream.cpp
    internal/NodeConverter.cpp
    internal/StatementConverter.cpp
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

ADD_SUBDIRECTORY(ontology)
ADD_SUBDIRECTORY(codegen)
ADD_SUBDIRECTORY(xmi)
