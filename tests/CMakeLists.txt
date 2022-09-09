find_package( Boost COMPONENTS filesystem system)

set (GTEST googletest-release-1.7.0)

set(GTEST_SRC ${CMAKE_CURRENT_SOURCE_DIR}/${GTEST}/src/gtest-all.cc)
set(GTEST_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/${GTEST}/include ${CMAKE_CURRENT_SOURCE_DIR}/${GTEST})

include_directories(
    ${GTEST_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)

FILE (GLOB_RECURSE gtest_unittests_SRCS ../src/*UnitTests.cpp)
add_executable(
    gtest-all
    gtest-all.cpp
    ${gtest_unittests_SRCS}
    ${GTEST_SRC}
)
target_link_libraries (
    gtest-all
    autordf
    autordf-ontology
    ${CMAKE_THREAD_LIBS_INIT} #Required for google test
    ${Boost_LIBRARIES}
)

add_custom_target(check
    DEPENDS cmd_test)

ADD_CUSTOM_COMMAND(
    OUTPUT cmd_test
    COMMAND gtest-all --gtest_output=\"xml:${CMAKE_BINARY_DIR}/unittests.xml\"
)

if(GENERATE_PYTHON_BINDINGS)
    ADD_CUSTOM_COMMAND(
        OUTPUT cmd_test APPEND
        COMMAND LD_LIBRARY_PATH=${STAGING_PATH}/lib PYTHONPATH=${PYTHONPATH}:${STAGING_PATH}/lib python3 -m unittest ObjectUnitTests.py -vvv
    )
    IF(BUILD_TOOLS)
        ADD_CUSTOM_COMMAND(
            OUTPUT cmd_test APPEND
            COMMAND LD_LIBRARY_PATH=${STAGING_PATH}/lib PYTHONPATH=${PYTHONPATH}:${STAGING_PATH}/lib python3 -m unittest CodegenUnitTests.py -vvv
        )
    endif()
endif()

add_executable(
    perftest
    perftest.cpp
)

target_link_libraries (
    perftest
    autordf
    ${Boost_LIBRARIES}
)
