cmake_minimum_required(VERSION 4.0)

project(vkGuide LANGUAGES C CXX)

# --- C++ ---
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set(CMAKE_CXX_COMPILER /usr/bin/clang++)

# --- Dependencies ---
find_package(Vulkan REQUIRED)
find_package(volk REQUIRED)
find_package(VulkanMemoryAllocator REQUIRED)
find_package(SDL3 REQUIRED)
find_package(glm REQUIRED)

# --- External libraries ---
add_subdirectory("external/fastgltf")
add_subdirectory("external/vk-bootstrap")
add_subdirectory("external/fmt")


# ImGui
set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/external/imgui")
set(IMGUI_SOURCES
  "${IMGUI_DIR}/imgui.cpp"
  "${IMGUI_DIR}/imgui_draw.cpp"
  "${IMGUI_DIR}/imgui_tables.cpp"
  "${IMGUI_DIR}/imgui_widgets.cpp"
  "${IMGUI_DIR}/imgui_demo.cpp"

  "${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp"
  "${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp"
)
add_library(imgui STATIC ${IMGUI_SOURCES})

target_include_directories(imgui PUBLIC
  "${IMGUI_DIR}"
  "${IMGUI_DIR}/backends"
)

target_link_libraries(imgui PUBLIC
  Vulkan::Vulkan
  SDL3::SDL3
)

# Main executable
file(GLOB_RECURSE SRC CONFIGURE_DEPENDS
  "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

add_executable(${PROJECT_NAME} ${SRC})

target_include_directories(${PROJECT_NAME} PRIVATE
  "${CMAKE_SOURCE_DIR}/src"
)

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

target_compile_options(${PROJECT_NAME} PRIVATE
  -Wall
  -Werror
)

# Shader compilation
find_program(GLSL_LANG_VALIDATOR
  NAMES glslangValidator glslangvalidator
  PATHS /usr/local/bin
)

if(NOT GLSL_LANG_VALIDATOR)
  message(FATAL_ERROR
    "glslangValidator was not found. "
    "Please install the Vulkan shader tools."
  )
endif()

message(STATUS "Using glslangValidator: ${GLSL_LANG_VALIDATOR}")

# Output directory for compiled SPIR-V
set(SPIRV_OUTPUT_DIR
  "${CMAKE_BINARY_DIR}/shaders"
)

file(MAKE_DIRECTORY "${SPIRV_OUTPUT_DIR}")

# Find all shader files
file(GLOB_RECURSE GLSL_SRC CONFIGURE_DEPENDS
  "${PROJECT_SOURCE_DIR}/shaders/*.glsl"
  "${PROJECT_SOURCE_DIR}/shaders/*.vert"
  "${PROJECT_SOURCE_DIR}/shaders/*.frag"
  "${PROJECT_SOURCE_DIR}/shaders/*.comp"
)

set(SPIRV_FILES)

foreach(GLSL_SHADER IN LISTS GLSL_SRC)

  get_filename_component(FILE_NAME
    "${GLSL_SHADER}"
    NAME
  )

  set(SPIRV
    "${SPIRV_OUTPUT_DIR}/${FILE_NAME}.spv"
  )

  message(STATUS "Shader: ${GLSL_SHADER}")
  message(STATUS "     -> ${SPIRV}")

  add_custom_command(
    OUTPUT "${SPIRV}"
    COMMAND "${GLSL_LANG_VALIDATOR}" -V "${GLSL_SHADER}" -o "${SPIRV}"
    DEPENDS "${GLSL_SHADER}"
    COMMENT "Compiling shader ${FILE_NAME}"
    VERBATIM
  )

  list(APPEND SPIRV_FILES "${SPIRV}")

endforeach()


# Shader target
add_custom_target(shaders
  DEPENDS ${SPIRV_FILES}
)


# Make the executable depend on the shader target
add_dependencies(${PROJECT_NAME} shaders)
