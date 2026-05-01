find_package (Threads REQUIRED)

if (DEFINED VCPKG_TOOLCHAIN)
    find_package (ada CONFIG REQUIRED)
    find_package (asio CONFIG REQUIRED)
    if (PARS_ENABLE_LOGGING)
        find_package (spdlog CONFIG REQUIRED)
    else ()
        find_package (fmt CONFIG REQUIRED)
    endif ()
    find_package (flatbuffers CONFIG REQUIRED)
else ()
    CPMAddPackage (asio
        GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
        GIT_TAG asio-1-38-0
        DOWNLOAD_ONLY YES
    )
    if (NOT TARGET asio::asio)
        add_library (asio::asio INTERFACE IMPORTED)
        target_include_directories (asio::asio INTERFACE ${asio_SOURCE_DIR}/include)
        target_compile_definitions (asio::asio INTERFACE ASIO_STANDALONE)
    endif ()
    CPMAddPackage (flatbuffers
      GIT_REPOSITORY https://github.com/google/flatbuffers.git
      GIT_TAG v25.12.19
      OPTIONS
      "FLATBUFFERS_BUILD_FLATLIB OFF"
      "FLATBUFFERS_BUILD_FLATC OFF"
      "FLATBUFFERS_BUILD_FLATHASH OFF"
      "FLATBUFFERS_BUILD_TESTS OFF"
      "FLATBUFFERS_INSTALL OFF"
    )
    if (PARS_ENABLE_LOGGING)
        CPMAddPackage (spdlog
            GIT_REPOSITORY https://github.com/gabime/spdlog.git
            GIT_TAG v1.17.0
            OPTIONS
            "SPDLOG_USE_STD_FORMAT ON"
        )
    else ()
        CPMAddPackage (fmt
            GIT_REPOSITORY https://github.com/fmtlib/fmt.git
            GIT_TAG 12.1.0
            OPTIONS
            "FMT_INSTALL OFF"
        )
    endif ()
endif ()

# sorted please
set (PARS_HEADERS
    "include/clev/err.h"
    "include/clev/iface.h"
    "include/clev/own.h"
    "include/clev/value.h"
    "include/pars/app/resources.h"
    "include/pars/app/setup.h"
    "include/pars/app/single.h"
    "include/pars/app/state_machine.h"
    "include/pars/comp/client.h"
    "include/pars/concept/event.h"
    "include/pars/concept/formattable.h"
    "include/pars/concept/hashable.h"
    "include/pars/concept/kind.h"
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
    "include/pars/fmt/stl.h"
    "include/pars/log/demangle.h"
    "include/pars/log/flags.h"
    "include/pars/log/nametype.h"
    "include/pars/net/asio.h"
    "include/pars/net/connect_mode.h"
    "include/pars/net/dialer.h"
    "include/pars/net/direction.h"
    "include/pars/net/hash.h"
    "include/pars/net/io.h"
    "include/pars/net/listener.h"
    "include/pars/net/msg.h"
    "include/pars/net/pipe.h"
    "include/pars/net/point.h"
    "include/pars/net/resolver.h"
    "include/pars/err.h"
    "include/pars/fmt.h"
    "include/pars/init.h"
    "include/pars/log.h"
)

add_library (pars INTERFACE ${PARS_HEADERS})
add_library (pars::pars ALIAS pars)

target_sources (
    pars INTERFACE
    FILE_SET HEADERS
    BASE_DIRS include
    FILES ${PARS_HEADERS}
)

target_link_libraries (
    pars
    INTERFACE
    asio::asio
    FlatBuffers
)

if (PARS_ENABLE_LOGGING)
    target_compile_definitions (pars INTERFACE PARS_LOGGING_ENABLED)
    target_link_libraries (
        pars
        INTERFACE
        spdlog::spdlog
    )
else ()
     target_link_libraries (pars INTERFACE fmt::fmt-header-only)
endif ()

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

## install
if (DEFINED VCPKG_TOOLCHAIN)
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

    install (
        FILES ${CMAKE_BINARY_DIR}/lib/generated/config.h
        DESTINATION include/pars
    )

    export (
        TARGETS pars
        NAMESPACE pars::
        FILE "${CMAKE_CURRENT_BINARY_DIR}/${pars_config_targets_file}"
    )

    install (
        EXPORT pars
        DESTINATION ${pars_export_dest_dir}
        NAMESPACE pars::
        FILE ${pars_config_targets_file}
    )

    include (CMakePackageConfigHelpers)
    configure_package_config_file (
        "${pars_project_config_in}"
        "${pars_project_config_out}"
        INSTALL_DESTINATION ${pars_export_dest_dir}
    )

    write_basic_package_version_file (
        "${pars_version_config_file}"
        COMPATIBILITY SameMajorVersion
    )

    install (
        FILES "${pars_project_config_out}" "${pars_version_config_file}"
        DESTINATION "${pars_export_dest_dir}"
    )
endif ()
