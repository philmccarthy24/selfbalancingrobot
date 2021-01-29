#find_package(GTest REQUIRED)
find_package(GTest CONFIG REQUIRED)

# from vcpkg install cmd:
#find_package(GTest CONFIG REQUIRED)
#target_link_libraries(main PRIVATE GTest::gmock GTest::gtest GTest::gmock_main GTest::gtest_main)

include(GoogleTest)

add_executable(tests ExampleTest.cpp)
#target_link_libraries(tests GTest::GTest GTest::Main)
target_link_libraries(tests PRIVATE GTest::gmock GTest::gtest GTest::gmock_main GTest::gtest_main)
gtest_discover_tests(tests)

add_test (NAME tests COMMAND tests) # how does this work?