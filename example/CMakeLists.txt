# SINGLE

include("enable_sanitizer")

find_package(spdlog CONFIG REQUIRED)

add_executable(client
  "client.cpp"
  "common.h"
  "event.h"
)
target_link_libraries(client PUBLIC pars spdlog::spdlog)
target_include_directories(client PUBLIC pars spdlog::spdlog)
# enable_sanitizer (client address)
# enable_sanitizer (client thread)

add_executable(server_backend
  "server_backend.cpp"
  "common.h"
  "event.h"
  "fib.h"
)
target_link_libraries(server_backend PUBLIC pars spdlog::spdlog)
target_include_directories(server_backend PUBLIC pars spdlog::spdlog)
# enable_sanitizer (server_backend address)
# enable_sanitizer (server_backend thread)
