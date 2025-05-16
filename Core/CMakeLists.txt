file(GLOB_RECURSE RESOURCE  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
set_source_files_properties(${SOURCE_FILES} PROPERTIES CMAKE_CONFIGURE_DEPENDS ON)

find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_library(Core ${RESOURCE})
target_include_directories(Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(Core PRIVATE glad::glad)
target_link_libraries(Core PRIVATE glm::glm)
target_link_libraries(Core PUBLIC Common)