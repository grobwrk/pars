# set (THREADS_PREFER_PTHREAD_FLAG ON)
find_package (Threads REQUIRED)
find_package (fmt CONFIG REQUIRED)
find_package (nng CONFIG REQUIRED)
find_package (spdlog CONFIG REQUIRED)

add_library (pars INTERFACE)
add_library (pars::pars ALIAS pars)

# sorted please
target_sources (
    pars INTERFACE
    FILE_SET HEADERS BASE_DIRS include
    FILES
    "include/pars/app/resources.h"
    "include/pars/app/setup.h"
    "include/pars/app/single.h"
    "include/pars/app/state_machine.h"
    "include/pars/comp/backend.h"
    "include/pars/comp/client.h"
    "include/pars/concept/event.h"
    "include/pars/concept/formattable.h"
    "include/pars/concept/hashable.h"
    "include/pars/concept/kind.h"
    "include/pars/concept/net.h"
    "include/pars/ev/dispatcher.h"
    "include/pars/ev/enqueuer.h"
    "include/pars/ev/event.h"
    "include/pars/ev/hf_registry.h"
    "include/pars/ev/hf_registry__insert.h"
    "include/pars/ev/job.h"
    "include/pars/ev/kind.h"
    "include/pars/ev/kind_base.h"
    "include/pars/ev/kind_decl.h"
    "include/pars/ev/klass.h"
    "include/pars/ev/make_hf.h"
    "include/pars/ev/metadata.h"
    "include/pars/ev/runner.h"
    "include/pars/ev/serializer.h"
    "include/pars/ev/spec.h"
    "include/pars/fmt/formattable.h"
    "include/pars/fmt/helpers.h"
    "include/pars/fmt/nng.h"
    "include/pars/fmt/stl.h"
    "include/pars/log/demangle.h"
    "include/pars/log/flags.h"
    "include/pars/log/nametype.h"
    "include/pars/net/context.h"
    "include/pars/net/context_opt.h"
    "include/pars/net/context_registry.h"
    "include/pars/net/dir.h"
    "include/pars/net/hash.h"
    "include/pars/net/op.h"
    "include/pars/net/pipe.h"
    "include/pars/net/pull.h"
    "include/pars/net/push.h"
    "include/pars/net/rep.h"
    "include/pars/net/req.h"
    "include/pars/net/socket.h"
    "include/pars/net/socket_opt.h"
    "include/pars/net/tool_view.h"
    "include/pars/err.h"
    "include/pars/init.h"
    "include/pars/log.h"
    "include/pars/pars.h"
)

target_link_libraries (
    pars
    INTERFACE
    nng::nng
    fmt::fmt
    spdlog::spdlog
)

target_include_directories (
    pars
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/lib/generated/>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
    $<INSTALL_INTERFACE:include/>
)

# generate config.h
string (TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
configure_file ("config.h.in" "generated/config.h")

# install
include (GNUInstallDirs)

set (pars_project_config_in "${CMAKE_SOURCE_DIR}/cmake/parsConfig.cmake.in")
set (pars_project_config_out "${CMAKE_CURRENT_BINARY_DIR}/parsConfig.cmake")
set (pars_config_targets_file "parsConfigTargets.cmake")
set (pars_version_config_file "${CMAKE_CURRENT_BINARY_DIR}/parsConfigVersion.cmake")
set (pars_export_dest_dir "${CMAKE_INSTALL_LIBDIR}/cmake/pars")

install (
    TARGETS pars
    EXPORT pars
    LIBRARY FILE_SET HEADERS
)

install(
    FILES ${CMAKE_BINARY_DIR}/lib/generated/config.h
    DESTINATION include/pars
)

export(
    TARGETS pars
    NAMESPACE pars::
    FILE "${CMAKE_CURRENT_BINARY_DIR}/${pars_config_targets_file}"
)

install(
    EXPORT pars
    DESTINATION ${pars_export_dest_dir}
    NAMESPACE pars::
    FILE ${pars_config_targets_file}
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${pars_project_config_in}"
    "${pars_project_config_out}"
    INSTALL_DESTINATION ${pars_export_dest_dir}
)

write_basic_package_version_file(
    "${pars_version_config_file}"
    COMPATIBILITY SameMajorVersion
)

install(
    FILES "${pars_project_config_out}" "${pars_version_config_file}"
    DESTINATION "${pars_export_dest_dir}"
)
