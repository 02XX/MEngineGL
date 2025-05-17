find_package(GTest CONFIG REQUIRED)

add_executable(UUIDTest UUIDTest.cpp)
add_test(NAME UUIDTest COMMAND UUIDTest)
target_link_libraries(UUIDTest PUBLIC Core GTest::gtest GTest::gtest_main)