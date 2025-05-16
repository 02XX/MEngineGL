find_package(GTest CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
add_executable(ShaderManagerTest ShaderManagerTest.cpp)
add_test(NAME ShaderManagerTest COMMAND ShaderManagerTest)
target_link_libraries(ShaderManagerTest PUBLIC Core GTest::gtest GTest::gtest_main glfw glad::glad)


