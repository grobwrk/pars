# FlatBuffers schema compilation
find_package(flatbuffers REQUIRED CONFIG)

# Find all .fbs schema files in this directory
file(GLOB SCHEMA_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.fbs")

# Directory for generated headers (in build folder)
set(GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
file(MAKE_DIRECTORY ${GENERATED_DIR})

# Generate C++ headers from FlatBuffers schemas
set(GENERATED_HEADERS "")

foreach(SCHEMA_FILE ${SCHEMA_FILES})
    get_filename_component(SCHEMA_NAME ${SCHEMA_FILE} NAME_WE)
    set(GENERATED_HEADER "${GENERATED_DIR}/${SCHEMA_NAME}_generated.h")

    add_custom_command(
        OUTPUT ${GENERATED_HEADER}
        COMMAND $<TARGET_FILE:flatbuffers::flatc> -c -o ${GENERATED_DIR} ${SCHEMA_FILE}
        DEPENDS ${SCHEMA_FILE}
        COMMENT "Generating FlatBuffers C++ code for ${SCHEMA_NAME}.fbs"
        VERBATIM
    )

    list(APPEND GENERATED_HEADERS ${GENERATED_HEADER})
endforeach()

# Create a custom target that depends on all generated headers
add_custom_target(generate_flatbuffers ALL
    DEPENDS ${GENERATED_HEADERS}
    COMMENT "Generating all FlatBuffers headers"
)

# Export the generated directory to parent scope
set(FLATBUFFERS_GENERATED_DIR ${GENERATED_DIR} PARENT_SCOPE)
set(FLATBUFFERS_GENERATED_HEADERS ${GENERATED_HEADERS} PARENT_SCOPE)
