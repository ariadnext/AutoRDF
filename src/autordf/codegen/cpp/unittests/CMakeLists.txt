INCLUDE_DIRECTORIES(
    "${PROJECT_SOURCE_DIR}/include"
)

ADD_EXECUTABLE(
    testcodegen
    TestCodeGen.cpp
    "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/IAgent.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/IDocument.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/IGroup.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/IImage.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/ILabelProperty.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/IOnlineAccount.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/IOnlineChatAccount.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/IOnlineEcommerceAccount.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/IOnlineGamingAccount.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/IOrganization.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/IPerson.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/IPersonalProfileDocument.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/IProject.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaftest/rdfs/IResource.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaftest/rdfs/IClass.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaftest/owl/IThing.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/Agent.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/Document.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/Group.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/Image.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/LabelProperty.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/OnlineAccount.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/OnlineChatAccount.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/OnlineEcommerceAccount.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/OnlineGamingAccount.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/Organization.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/Person.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/PersonalProfileDocument.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/Project.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaftest/rdfs/Resource.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaftest/rdfs/Class.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/foaftest/owl/Thing.cpp"
)

TARGET_INCLUDE_DIRECTORIES(
    testcodegen
    PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/foaftest
)

TARGET_LINK_LIBRARIES(
    testcodegen
    autordf
    boost_system
    boost_filesystem
)

ADD_CUSTOM_COMMAND(
    OUTPUT
        "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/IAgent.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/IDocument.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/IGroup.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/IImage.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/ILabelProperty.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/IOnlineAccount.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/IOnlineChatAccount.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/IOnlineEcommerceAccount.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/IOnlineGamingAccount.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/IOrganization.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/IPerson.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/IPersonalProfileDocument.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/IProject.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaftest/rdfs/IResource.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaftest/rdfs/IClass.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaftest/owl/IThing.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/Agent.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/Document.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/Group.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/Image.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/LabelProperty.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/OnlineAccount.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/OnlineChatAccount.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/OnlineEcommerceAccount.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/OnlineGamingAccount.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/Organization.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/Person.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/PersonalProfileDocument.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaftest/foaf/Project.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaftest/rdfs/Resource.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaftest/rdfs/Class.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaftest/owl/Thing.cpp"
    COMMAND "autordfcodegen" -t "${CMAKE_SOURCE_DIR}/src/autordf/codegen/templates" -o foaftest "${CMAKE_CURRENT_SOURCE_DIR}/foaf_20140114.ttl"
    DEPENDS "autordfcodegen" ${AUTORDF_TEMPLATES}
)

ADD_EXECUTABLE(
    testmrd
    TestMrd.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/testmrd_output/AllInOne.cpp
)

TARGET_INCLUDE_DIRECTORIES(
    testmrd
    PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/testmrd_output
)

TARGET_LINK_LIBRARIES(
    testmrd
    autordf
)

ADD_CUSTOM_COMMAND(
    OUTPUT
    ${CMAKE_CURRENT_BINARY_DIR}/testmrd_output/AllInOne.cpp
    COMMAND "autordfcodegen" -a -t "${CMAKE_SOURCE_DIR}/src/autordf/codegen/templates" -o testmrd_output "${CMAKE_CURRENT_SOURCE_DIR}/MachineReadableDocument.ttl"
    DEPENDS "autordfcodegen" "${CMAKE_CURRENT_SOURCE_DIR}/MachineReadableDocument.ttl" ${AUTORDF_TEMPLATES}
)
