file(GLOB_RECURSE RESOURCE  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
set_source_files_properties(${SOURCE_FILES} PROPERTIES CMAKE_CONFIGURE_DEPENDS ON)

add_library(Resource ${RESOURCE})
target_include_directories(Resource PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(Resource PUBLIC Core)

#Shader
add_custom_target(
    Shader
    ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/Shader
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/Shader ${CMAKE_BINARY_DIR}/bin/Shader
    COMMENT "Copying shader files from source_folder to destination_folder..."
    VERBATIM
)
add_dependencies(Resource Shader)

#appsetting.json
add_custom_target(
    AppSetting
    ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/appsettings.json ${CMAKE_BINARY_DIR}/bin/appsettings.json
    COMMENT "Copying appsetting.json files from source_folder to destination_folder..."
    VERBATIM
)
add_dependencies(Resource AppSetting)