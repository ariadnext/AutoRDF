IF(APPLE AND NOT NATIVE)
set(Boost_INCLUDE_DIRS ${CMAKE_FRAMEWORK_EXTRA_PATH}/boost.framework)
set(Boost_LIBRARIES boost)
message(STATUS "FORCED Boost_INCLUDE_DIRS to ${Boost_INCLUDE_DIRS}")
ELSE(APPLE AND NOT NATIVE)
find_package(Boost REQUIRED COMPONENTS date_time)
ENDIF(APPLE AND NOT NATIVE)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

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
    Storage.cpp
    internal/World.cpp
    internal/ModelPrivate.cpp
    internal/Parser.cpp
    internal/Uri.cpp
    internal/Stream.cpp
    internal/StatementConverter.cpp
    cvt/RdfTypeEnum.cpp
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

if(${BUILD_TOOLS} STREQUAL "1")
ADD_SUBDIRECTORY(codegen)
ADD_SUBDIRECTORY(xmi)
endif()