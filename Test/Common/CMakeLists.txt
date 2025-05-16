find_package(GTest CONFIG REQUIRED)

add_executable(LoggerTest LoggerTest.cpp)
add_test(NAME LoggerTest COMMAND LoggerTest)
target_link_libraries(LoggerTest PUBLIC Common GTest::gtest GTest::gtest_main)

add_executable(ConfigureTest ConfigureTest.cpp)
add_test(NAME ConfigureTest COMMAND ConfigureTest)
target_link_libraries(ConfigureTest PUBLIC Common GTest::gtest GTest::gtest_main)



