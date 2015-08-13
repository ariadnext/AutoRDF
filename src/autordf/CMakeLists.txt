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
    ResourceList.cpp
    Property.cpp
    Factory.cpp
    Object.cpp
)
TARGET_LINK_LIBRARIES(
    autordf
    ${REDLAND_LIBRARIES}
)
SET_TARGET_PROPERTIES(autordf PROPERTIES VERSION ${SOLIBVERSION})

INSTALL(TARGETS autordf
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

INSTALL(FILES
    ../../include/autordf/Factory.h
    ../../include/autordf/Model.h
    ../../include/autordf/Node.h
    ../../include/autordf/NodeType.h
    ../../include/autordf/Object.h
    ../../include/autordf/Property.h
    ../../include/autordf/PropertyValue.h
    ../../include/autordf/Resource.h
    ../../include/autordf/ResourceList.h
    ../../include/autordf/Statement.h
    ../../include/autordf/StatementList.h
    DESTINATION include/autordf
)

ADD_SUBDIRECTORY(tools)