find_path(GTEST_INCLUDE_DIRS gtest.h PATH_SUFFIXES include/gtest)
message(STATUS "Found Google Test headers: ${GTEST_INCLUDE_DIR}")

find_package( Boost COMPONENTS filesystem system)

set(GTEST_SRC /usr/src/gtest/src/gtest-all.cc)

include_directories(
    ${GTEST_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    /usr/src/gtest/
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
    ${GTEST_LIBRARIES}
    ${Boost_LIBRARIES}
)