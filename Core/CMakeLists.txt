file(GLOB_RECURSE RESOURCE  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
set_source_files_properties(${SOURCE_FILES} PROPERTIES CMAKE_CONFIGURE_DEPENDS ON)

find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
add_library(Core ${RESOURCE})
target_include_directories(Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(Core PUBLIC glad::glad)
target_link_libraries(Core PUBLIC glm::glm)
target_link_libraries(Core PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(Core PUBLIC Common)