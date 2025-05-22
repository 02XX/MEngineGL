find_package(GTest CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)


add_executable(UUIDTest UUIDTest.cpp)
add_test(NAME UUIDTest COMMAND UUIDTest)
target_link_libraries(UUIDTest PUBLIC Core GTest::gtest GTest::gtest_main)

add_executable(Texture2DTest Texture2DTest.cpp)
add_test(NAME Texture2DTest COMMAND Texture2DTest)
target_link_libraries(Texture2DTest PUBLIC Core GTest::gtest GTest::gtest_main glfw glad::glad)

add_executable(SerializeTest SerializeTest.cpp)
add_test(NAME SerializeTest COMMAND SerializeTest)
target_link_libraries(SerializeTest PUBLIC  GTest::gtest GTest::gtest_main nlohmann_json::nlohmann_json)

add_executable(ReflTest ReflTest.cpp)
add_test(NAME ReflTest COMMAND ReflTest)
target_link_libraries(ReflTest PUBLIC  GTest::gtest GTest::gtest_main EnTT::EnTT)
