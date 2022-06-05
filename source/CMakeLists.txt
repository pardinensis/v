# the main executable has the same name as the CMake project
# (see project() call in the main CMakeLists.txt)
set(EXECUTABLE_NAME ${PROJECT_NAME})
add_executable(${EXECUTABLE_NAME})

target_sources(${EXECUTABLE_NAME} PRIVATE
    app.cpp
    config.cpp
    main.cpp
    vulkan_device.cpp
    vulkan_swapchain.cpp
)

target_include_directories(${EXECUTABLE_NAME} PRIVATE
    ../include
)

target_compile_features(${EXECUTABLE_NAME} PRIVATE
    cxx_std_20
)

target_compile_options(${EXECUTABLE_NAME} PRIVATE
    -Werror -Wall -Wextra -pedantic
)

target_link_libraries(${EXECUTABLE_NAME} PRIVATE
    ${SDL2_LIBRARIES}
    ${Vulkan_LIBRARY}
    ${YAML_CPP_LIBRARIES}
)

# install(TARGETS ${EXECUTABLE_NAME})
