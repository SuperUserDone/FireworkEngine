enable_testing()

add_executable(fw-test sanity_check.cpp "core/serialize/scene_serialize.cpp")
target_link_libraries(fw-test firework-engine GTest::gmock GTest::gtest
                      GTest::gmock_main GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(fw-test)
