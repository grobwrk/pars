enable_testing ()

find_package (GTest CONFIG REQUIRED)
include (GoogleTest)

set (
  PARS_TESTS
  "events-internal"
  "events-network"
)

foreach (TEST ${PARS_TESTS})
  set (TEST_TARGET "test-${TEST}")
  add_executable (${TEST_TARGET} "${TEST}.cpp")
  target_link_libraries (${TEST_TARGET} PUBLIC pars)

  if (${PARS_BUILD_COVERAGE})
    target_compile_options (${TEST_TARGET} PRIVATE "-fprofile-instr-generate" "-fcoverage-mapping")
    target_link_options (${TEST_TARGET} PRIVATE "-fprofile-instr-generate" "-fcoverage-mapping")
  endif ()

  # Putting either GTest::gmock or GTest::gmock_main or both, broke tests
  target_link_libraries (${TEST_TARGET} PRIVATE GTest::gtest GTest::gtest_main)
endforeach ()

add_executable (tests "events-internal.cpp" "events-network.cpp")
target_link_libraries (tests PUBLIC pars)
if (${PARS_BUILD_COVERAGE})
  target_compile_options (tests PRIVATE "-fprofile-instr-generate" "-fcoverage-mapping")
  target_link_options (tests PRIVATE "-fprofile-instr-generate" "-fcoverage-mapping")
endif ()
target_link_libraries (tests PRIVATE GTest::gtest GTest::gtest_main)
gtest_discover_tests (tests)
