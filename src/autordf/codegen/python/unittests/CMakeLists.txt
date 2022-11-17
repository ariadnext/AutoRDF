ADD_CUSTOM_COMMAND(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/example/ex/ObjectA.py
    COMMAND "autordfcodegen" -g python -t "${CMAKE_SOURCE_DIR}/src/autordf/codegen/templates" -n "ex:http://www.semanticweb.org/qdauchy/ontologies/2022/8/example#" -o example "${CMAKE_CURRENT_SOURCE_DIR}/example.ttl"
    DEPENDS "autordfcodegen" "${CMAKE_CURRENT_SOURCE_DIR}/example.ttl" "${AUTORDF_TEMPLATES}"
)
ADD_CUSTOM_TARGET(fake_test ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/example/ex/ObjectA.py)
INSTALL(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/example DESTINATION ${CMAKE_BINARY_DIR}/tests)
INSTALL(FILES CodegenUnitTests.py DESTINATION ${CMAKE_BINARY_DIR}/tests/)
