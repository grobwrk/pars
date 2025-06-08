enable_testing()

find_package(GTest CONFIG REQUIRED)
include(GoogleTest)

set(
  PARS_TESTS
  "ev-event--internal.cpp"
  "ev-event--network.cpp"
  "nngxx-msg.cpp"
)

foreach (TEST_SOURCE ${PARS_TESTS})
  string(REPLACE ".cpp" "" TEST_NAME ${TEST_SOURCE})
  set(TEST_TARGET "test-${TEST_NAME}")
  file(GLOB TEST_HEADERS "${TEST_NAME}*.h")

  add_executable(${TEST_TARGET} ${TEST_SOURCE} ${TEST_HEADERS})
  target_link_libraries(${TEST_TARGET} PUBLIC pars)

  if (${PARS_BUILD_COVERAGE})
    target_compile_options(${TEST_TARGET} PRIVATE "-fprofile-instr-generate" "-fcoverage-mapping")
    target_link_options(${TEST_TARGET} PRIVATE "-fprofile-instr-generate" "-fcoverage-mapping")
  endif ()

  # Putting either GTest::gmock or GTest::gmock_main or both, broke tests
  target_link_libraries(${TEST_TARGET} PRIVATE GTest::gtest GTest::gtest_main)
endforeach ()

add_executable(tests ${PARS_TESTS})
target_link_libraries(tests PUBLIC pars)
if (${PARS_BUILD_COVERAGE})
  target_compile_options(tests PRIVATE "-fprofile-instr-generate" "-fcoverage-mapping")
  target_link_options(tests PRIVATE "-fprofile-instr-generate" "-fcoverage-mapping")
endif ()
target_link_libraries(tests PRIVATE GTest::gtest GTest::gtest_main)
gtest_discover_tests(tests)
