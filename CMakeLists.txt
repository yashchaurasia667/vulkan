cmake_minimum_required(VERSION 4.0)
project(vkGuide LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_COMPILER /usr/bin/clang++)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Vulkan REQUIRED)
find_package(volk REQUIRED)
find_package(VulkanMemoryAllocator REQUIRED)
find_package(SDL3 REQUIRED)
find_package(glm REQUIRED)

add_subdirectory("external/fastgltf")
add_subdirectory("external/vk-bootstrap")
add_subdirectory("external/fmt")

# --- ImGui ---
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/external/imgui)
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp
)
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    /usr/local/include
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(imgui PUBLIC Vulkan::Vulkan SDL3::SDL3)

# --- Main executable ---
file(GLOB_RECURSE SRC "${CMAKE_SOURCE_DIR}/src/*.cpp")
add_executable(${PROJECT_NAME} ${SRC})

target_link_libraries(${PROJECT_NAME} PRIVATE
    Vulkan::Vulkan
    volk::volk
    GPUOpen::VulkanMemoryAllocator
    SDL3::SDL3
    glm::glm
    fastgltf
    vk-bootstrap::vk-bootstrap
    imgui
    fmt::fmt
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Werror)
