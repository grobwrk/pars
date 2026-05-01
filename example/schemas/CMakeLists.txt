# FlatBuffers schema compilation
if (DEFINED VCPKG_TOOLCHAIN)
    find_package(flatbuffers REQUIRED CONFIG)
else ()
    FetchContent_GetProperties(flatbuffers SOURCE_DIR flatbuffers_SOURCE_DIR)
    message(STATUS "flatbuffers source dir: ${flatbuffers_SOURCE_DIR}")

    include(ExternalProject)
    ExternalProject_Add(flatc_host
        SOURCE_DIR      ${flatbuffers_SOURCE_DIR}
        CMAKE_ARGS
            -DCMAKE_BUILD_TYPE=Release
            -DFLATBUFFERS_BUILD_FLATLIB=OFF
            -DFLATBUFFERS_BUILD_TESTS=OFF
            -DFLATBUFFERS_INSTALL=OFF
        BUILD_BYPRODUCTS
            ${CMAKE_CURRENT_BINARY_DIR}/flatc_host-prefix/src/flatc_host-build/flatc
        INSTALL_COMMAND ""
    )

    # 4. Expose flatc as an imported executable
    ExternalProject_Get_Property(flatc_host BINARY_DIR)
    add_executable(flatc IMPORTED)
    set_target_properties(flatc PROPERTIES IMPORTED_LOCATION ${BINARY_DIR}/flatc)
    add_dependencies(flatc flatc_host)
endif ()

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
        COMMAND $<TARGET_FILE:flatc> -c -o ${GENERATED_DIR} ${SCHEMA_FILE}
        DEPENDS ${SCHEMA_FILE}
        COMMENT "Generating FlatBuffers C++ code for ${SCHEMA_NAME}.fbs"
        VERBATIM
    )

    list(APPEND GENERATED_HEADERS ${GENERATED_HEADER})
endforeach()

# Create a custom target that depends on all generated headers
add_custom_target(events ALL
    DEPENDS ${GENERATED_HEADERS}
    SOURCES ${SCHEMA_FILES}
    COMMENT "Generating all FlatBuffers headers"
)

# Export the generated directory to parent scope
set(FLATBUFFERS_GENERATED_DIR ${GENERATED_DIR} PARENT_SCOPE)
set(FLATBUFFERS_GENERATED_HEADERS ${GENERATED_HEADERS} PARENT_SCOPE)
