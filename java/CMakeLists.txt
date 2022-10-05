find_package(JNI REQUIRED)

string(TOLOWER ${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR} BINDING_SYSTEM_PATH)

set(JAVACPP_RELEASE_VERSION "1.5.7")
set(JAVACPP_RELEASE_NAME "javacpp-platform-${JAVACPP_RELEASE_VERSION}-bin")
set(JAVACPP_RELEASE_URL "https://github.com/bytedeco/javacpp/releases/download/${JAVACPP_RELEASE_VERSION}/${JAVACPP_RELEASE_NAME}.zip")

set(BOUND_HEADERS ${CMAKE_SOURCE_DIR}/include/autordf/Object.h)

# Fetching JavaCpp release
file(DOWNLOAD ${JAVACPP_RELEASE_URL} ${CMAKE_CURRENT_BINARY_DIR}/${JAVACPP_RELEASE_NAME}.zip SHOW_PROGRESS)

# Extracting JavaCpp
file(ARCHIVE_EXTRACT INPUT ${CMAKE_CURRENT_BINARY_DIR}/${JAVACPP_RELEASE_NAME}.zip DESTINATION ${CMAKE_CURRENT_BINARY_DIR} VERBOSE)

# Configuration compilation
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/com/github/ariadnext/autordf/AutoRdfConfig.class
    COMMAND javac -cp ${CMAKE_CURRENT_BINARY_DIR}/${JAVACPP_RELEASE_NAME}/javacpp.jar -d ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/src/main/com/github/ariadnext/autordf/AutoRdfConfig.java
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/main/com/github/ariadnext/autordf/AutoRdfConfig.java ${CMAKE_CURRENT_BINARY_DIR}/${JAVACPP_RELEASE_NAME}/javacpp.jar
)

# Binding generation
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/com/github/ariadnext/autordf/AutoRdf.java
    COMMAND java -jar ${CMAKE_CURRENT_BINARY_DIR}/${JAVACPP_RELEASE_NAME}/javacpp.jar -Dplatform.includepath=${CMAKE_SOURCE_DIR}/include/autordf -d ${CMAKE_CURRENT_BINARY_DIR} -nocompile com.github.ariadnext.autordf.AutoRdfConfig
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/com/github/ariadnext/autordf/AutoRdfConfig.class ${BOUND_HEADERS}
)

# Binding compilation
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/com/github/ariadnext/autordf/AutoRdf.class
    COMMAND javac -cp ${CMAKE_CURRENT_BINARY_DIR}/${JAVACPP_RELEASE_NAME}/javacpp.jar:${CMAKE_CURRENT_BINARY_DIR} -d ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/com/github/ariadnext/autordf/AutoRdf.java
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/com/github/ariadnext/autordf/AutoRdf.java
)

# JNI generation
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/jniAutoRdf.cpp
    BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/jnijavacpp.cpp
    COMMAND java -jar ${CMAKE_CURRENT_BINARY_DIR}/${JAVACPP_RELEASE_NAME}/javacpp.jar -d ${CMAKE_CURRENT_BINARY_DIR} -nocompile com.github.ariadnext.autordf.AutoRdf
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/com/github/ariadnext/autordf/AutoRdf.class
)

# JNI library compilation
add_library(jniAutoRdf SHARED
    ${CMAKE_CURRENT_BINARY_DIR}/jnijavacpp.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/jniAutoRdf.cpp
    )
target_include_directories(jniAutoRdf
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/autordf>
    ${JNI_INCLUDE_DIRS}
    )
target_link_libraries(jniAutoRdf
    PUBLIC
    ${JNI_LIBRARIES}
    ${PROJECT_NAME}
    )
