ADD_EXECUTABLE(
    testcodegen
    TestCodeGen.cpp
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/IAgent.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/IDocument.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/IGroup.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/IImage.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/ILabelProperty.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/IOnlineAccount.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/IOnlineChatAccount.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/IOnlineEcommerceAccount.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/IOnlineGamingAccount.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/IOrganization.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/IPerson.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/IPersonalProfileDocument.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/IProject.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/rdfs/IResource.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/rdfs/IClass.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/owl/IThing.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/Agent.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/Document.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/Group.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/Image.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/LabelProperty.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/OnlineAccount.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/OnlineChatAccount.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/OnlineEcommerceAccount.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/OnlineGamingAccount.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/Organization.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/Person.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/PersonalProfileDocument.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/Project.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/rdfs/Resource.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/rdfs/Class.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/owl/Thing.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/RdfTypeInfo.cpp"
)
SET_TARGET_PROPERTIES(testcodegen PROPERTIES INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/include;${CMAKE_CURRENT_BINARY_DIR}/foaf")

TARGET_LINK_LIBRARIES(
    testcodegen
    autordf
    boost_system
    boost_filesystem
)

ADD_CUSTOM_COMMAND(
    OUTPUT
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/IAgent.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/IDocument.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/IGroup.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/IImage.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/ILabelProperty.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/IOnlineAccount.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/IOnlineChatAccount.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/IOnlineEcommerceAccount.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/IOnlineGamingAccount.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/IOrganization.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/IPerson.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/IPersonalProfileDocument.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/IProject.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/rdfs/IResource.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/rdfs/IClass.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/owl/IThing.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/Agent.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/Document.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/Group.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/Image.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/LabelProperty.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/OnlineAccount.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/OnlineChatAccount.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/OnlineEcommerceAccount.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/OnlineGamingAccount.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/Organization.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/Person.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/PersonalProfileDocument.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/foaf/Project.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/rdfs/Resource.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/rdfs/Class.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/owl/Thing.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/foaf/RdfTypeInfo.cpp"
    COMMAND "${CMAKE_CURRENT_BINARY_DIR}/../autordfcodegen" -o foaf "${CMAKE_CURRENT_SOURCE_DIR}/foaf_20140114.rdf"
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/../autordfcodegen"
)

ADD_EXECUTABLE(
    testmrd
    TestMrd.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/mrd/AllInOne.cpp
)
SET_TARGET_PROPERTIES(testmrd PROPERTIES INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/include;${CMAKE_CURRENT_BINARY_DIR}/mrd")

TARGET_LINK_LIBRARIES(
    testmrd
    autordf
)

ADD_CUSTOM_COMMAND(
    OUTPUT
    ${CMAKE_CURRENT_BINARY_DIR}/mrd/AllInOne.cpp
    COMMAND "${CMAKE_CURRENT_BINARY_DIR}/../autordfcodegen" -a -o mrd -n "mrd:http://www.ariadnext.com/ontologies/2015/MachineReadableDocument/1.0.0#" "${CMAKE_CURRENT_SOURCE_DIR}/MachineReadableDocument.owl"
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/../autordfcodegen" "${CMAKE_CURRENT_SOURCE_DIR}/MachineReadableDocument.owl"
)
