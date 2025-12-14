# SINGLE

include ("enable_sanitizer")

add_subdirectory ("schemas")

# Mark generated headers so CMake doesn't expect them at configure time
set_source_files_properties(${FLATBUFFERS_GENERATED_HEADERS} PROPERTIES GENERATED TRUE)

add_executable (client
    "client.cpp"
    "common.h"
    "event.h"
    ${FLATBUFFERS_GENERATED_HEADERS}
)
add_dependencies (client generate_flatbuffers)
target_link_libraries (client PUBLIC pars)
target_include_directories (
    client
    PUBLIC pars
    PRIVATE ${FLATBUFFERS_GENERATED_DIR}
)
# enable_sanitizer (client address)
# enable_sanitizer (client thread)

add_executable (server_backend
    "server_backend.cpp"
    "common.h"
    "event.h"
    "fib.h"
    ${FLATBUFFERS_GENERATED_HEADERS}
)
add_dependencies (server_backend generate_flatbuffers)
target_link_libraries (server_backend PUBLIC pars)
target_include_directories (
    server_backend
    PUBLIC pars
    PRIVATE ${FLATBUFFERS_GENERATED_DIR}
)
# enable_sanitizer (server_backend address)
# enable_sanitizer (server_backend thread)
