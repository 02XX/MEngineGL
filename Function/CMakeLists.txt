file(GLOB_RECURSE RESOURCE  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
set_source_files_properties(${SOURCE_FILES} PROPERTIES CMAKE_CONFIGURE_DEPENDS ON)
find_package(EnTT CONFIG REQUIRED)

add_library(Function ${RESOURCE})
target_include_directories(Function PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(Function PUBLIC Resource)
target_link_libraries(Function PRIVATE EnTT::EnTT)
