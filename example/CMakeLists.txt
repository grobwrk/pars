# SINGLE

include ("enable_sanitizer")

add_subdirectory ("schemas")

# Mark generated headers so CMake doesn't expect them at configure time
set_source_files_properties(${FLATBUFFERS_GENERATED_HEADERS}
    PROPERTIES
    GENERATED TRUE)

add_executable (client
    "client.cpp"
    "common.h"
    "event.h"
    ${FLATBUFFERS_GENERATED_HEADERS}
)
add_dependencies (client events)
target_link_libraries (client PUBLIC pars)
target_include_directories (
    client
    PUBLIC pars
    PRIVATE ${FLATBUFFERS_GENERATED_DIR}
)
# enable_sanitizer (client address)
# enable_sanitizer (client thread)
