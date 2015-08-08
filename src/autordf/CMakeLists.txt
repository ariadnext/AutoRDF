ADD_LIBRARY(
    autordf
    ${LIBRARY_TYPE}
    World.cpp
    Model.cpp
    Storage.cpp
    ModelPrivate.cpp
    Parser Uri.cpp Uri.h)
TARGET_LINK_LIBRARIES(
    autordf
    ${REDLAND_LIBRARIES}
)
SET_TARGET_PROPERTIES(autordf PROPERTIES VERSION ${SOLIBVERSION})