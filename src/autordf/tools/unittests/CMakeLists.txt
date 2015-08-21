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
)
SET_TARGET_PROPERTIES(testcodegen PROPERTIES INCLUDE_DIRECTORIES "${CMAKE_CURRENT_BINARY_DIR}/foaf")

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
    COMMAND "${CMAKE_CURRENT_BINARY_DIR}/../autordfcodegen" -o foaf "${CMAKE_CURRENT_SOURCE_DIR}/foaf_20140114.rdf"
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/../autordfcodegen"
)

ADD_EXECUTABLE(
    testmrd
    TestMrd.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/mrd/AllInOne.cpp
)
SET_TARGET_PROPERTIES(testmrd PROPERTIES INCLUDE_DIRECTORIES "${CMAKE_CURRENT_BINARY_DIR}/mrd")

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
