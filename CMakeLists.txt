cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(Vulkan
    VERSION 0.1.0
    LANGUAGES CXX
)

find_package(glm REQUIRED)
find_package(SDL2 REQUIRED)
find_package(Vulkan REQUIRED)
find_package(yaml-cpp REQUIRED)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# create a link to the config file
add_custom_target(ConfigFile ALL)
add_custom_command(TARGET ConfigFile
    PRE_BUILD COMMAND ${CMAKE_COMMAND} -E create_symlink
    ${CMAKE_CURRENT_SOURCE_DIR}/config.yaml
    ${CMAKE_CURRENT_BINARY_DIR}/config.yaml
)

add_subdirectory(source)
