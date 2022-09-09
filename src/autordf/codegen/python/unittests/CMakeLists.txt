ADD_CUSTOM_COMMAND(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/example/ex/ObjectA.py
    COMMAND "autordfcodegen-python" -n "ex:http://www.semanticweb.org/qdauchy/ontologies/2022/8/example#" -o example "${CMAKE_CURRENT_SOURCE_DIR}/example.ttl"
    DEPENDS "autordfcodegen-python" "${CMAKE_CURRENT_SOURCE_DIR}/example.ttl"
)
ADD_CUSTOM_TARGET(fake_test ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/example/ex/ObjectA.py)
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/example DESTINATION ${CMAKE_BINARY_DIR}/tests/)
INSTALL(FILES CodegenUnitTests.py DESTINATION ${CMAKE_BINARY_DIR}/tests/)