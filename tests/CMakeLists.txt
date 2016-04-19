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
    pthread #Required for google test
    ${Boost_LIBRARIES}
)

add_custom_target(check
    COMMAND gtest-all
    DEPENDS gtest-all)
