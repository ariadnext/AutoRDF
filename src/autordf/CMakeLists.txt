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
    StatementList.cpp
StatementConverter.cpp StatementConverter.h)
TARGET_LINK_LIBRARIES(
    autordf
    ${REDLAND_LIBRARIES}
)
SET_TARGET_PROPERTIES(autordf PROPERTIES VERSION ${SOLIBVERSION})