INCLUDE_DIRECTORIES("${CMAKE_CURRENT_BINARY_DIR}")

SET_PROPERTY(SOURCE TestCodeGen.cpp APPEND PROPERTY OBJECT_DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf.h")

ADD_EXECUTABLE(
    testcodegen
    TestCodeGen.cpp
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/Agent.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/Document.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/Group.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/Image.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/LabelProperty.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/OnlineAccount.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/OnlineChatAccount.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/OnlineEcommerceAccount.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/OnlineGamingAccount.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/Organization.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/Person.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/PersonalProfileDocument.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/Project.cpp"
)

TARGET_LINK_LIBRARIES(
    testcodegen
    autordf
    boost_system
    boost_filesystem
)

ADD_CUSTOM_COMMAND(
    OUTPUT
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/Agent.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/Document.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/Group.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/Image.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/LabelProperty.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/OnlineAccount.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/OnlineChatAccount.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/OnlineEcommerceAccount.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/OnlineGamingAccount.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/Organization.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/Person.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/PersonalProfileDocument.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/Project.cpp"
    COMMAND "${CMAKE_CURRENT_BINARY_DIR}/../autordfcodegen" "${CMAKE_CURRENT_SOURCE_DIR}/foaf_20140114.rdf"
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/../autordfcodegen"
)
