ADD_EXECUTABLE(
    autordfcodegen
    AutoRdfCodeGen.cpp
)

TARGET_LINK_LIBRARIES (
    autordfcodegen
    autordf
)