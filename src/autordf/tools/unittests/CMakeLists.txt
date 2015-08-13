INCLUDE_DIRECTORIES("${CMAKE_CURRENT_BINARY_DIR}")

SET_PROPERTY(SOURCE TestCodeGen.cpp APPEND PROPERTY OBJECT_DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf.h")

ADD_EXECUTABLE(
    testcodegen
    TestCodeGen.cpp
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/IAgent.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/IDocument.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/IGroup.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/IImage.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/ILabelProperty.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/IOnlineAccount.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/IOnlineChatAccount.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/IOnlineEcommerceAccount.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/IOnlineGamingAccount.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/IOrganization.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/IPerson.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/IPersonalProfileDocument.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/IProject.cpp"
)

TARGET_LINK_LIBRARIES(
    testcodegen
    autordf
    boost_system
    boost_filesystem
)

ADD_CUSTOM_COMMAND(
    OUTPUT
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/IAgent.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/IDocument.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/IGroup.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/IImage.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/ILabelProperty.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/IOnlineAccount.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/IOnlineChatAccount.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/IOnlineEcommerceAccount.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/IOnlineGamingAccount.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/IOrganization.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/IPerson.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/IPersonalProfileDocument.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaf/IProject.cpp"
    COMMAND "${CMAKE_CURRENT_BINARY_DIR}/../autordfcodegen" "${CMAKE_CURRENT_SOURCE_DIR}/foaf_20140114.rdf"
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/../autordfcodegen"
)
