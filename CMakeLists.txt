# CMakeList.txt : CMake project for pars, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.30)
list (INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_LIST_DIR}/cmake")

set (PARS_VERSION "0.2.0")
project ("pars" VERSION ${PARS_VERSION} LANGUAGES CXX)

option (PARS_BUILD_EXAMPLES      "Build examples"      ON)
option (PARS_BUILD_DOCUMENTATION "Build documentation" OFF)
option (PARS_BUILD_TESTS         "Build tests"         OFF)
option (PARS_BUILD_COVERAGE      "Build coverage"      OFF) # Requires PARS_BUILD_TESTS=ON

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set (CMAKE_CXX_STANDARD 23)
  set (CMAKE_CXX_STANDARD_REQUIRED 1)
elseif (MSVC)
  # Enable Hot Reload for MSVC compilers if supported.
  if (POLICY CMP0141)
    cmake_policy (SET CMP0141 NEW)
    set (CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
  endif ()

  set (CMAKE_CXX_STANDARD 23)
  set (CMAKE_CXX_STANDARD_REQUIRED 1)
  add_compile_options ("/Zc:preprocessor")
  add_compile_options ("/Zc:__cplusplus")
else ()
  add_compile_options ("-std=c++23")
endif ()

# Add library
add_subdirectory (lib)

# Add binaries
add_subdirectory (bin)

# Add examples
if (${PARS_BUILD_EXAMPLES})
  add_subdirectory (example)
endif ()

# Add documentation
if (${PARS_BUILD_DOCUMENTATION})
  add_subdirectory (docs)
endif ()

# Add tests
if (${PARS_BUILD_TESTS})
  enable_testing ()
  add_subdirectory (test)
endif ()
