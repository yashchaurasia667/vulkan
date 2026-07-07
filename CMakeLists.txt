cmake_minimum_required(VERSION 4.2)
project(vulkanEngine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(EXTERN "${CMAKE_CURRENT_SOURCE_DIR}/extern")

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(volk CONFIG REQUIRED)
find_package(VulkanMemoryAllocator REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(tinyobjloader REQUIRED)

find_library(Slang_LIBRARY
  NAMES slang slang-compiler
  HINTS /usr/include
  REQUIRED
)

find_library(SHADERC_LIBRARY
  NAMES shaderc_shared
  HINTS /usr/include
)

set(KTX_DIR "${EXTERN}/ktx")

file(GLOB KTX_SRC "${KTX_DIR}/lib/*.c")
if(NOT KTX_SRC)
  message(FATAL_ERROR "No ktx sources found in ${KTX_DIR}/lib/")
endif()

add_library(ktx STATIC ${KTX_SRC})
target_compile_features(ktx PRIVATE c_std_99)
target_include_directories(ktx PUBLIC "${KTX_DIR}/include")
target_include_directories(ktx PUBLIC "${KTX_DIR}/other_include")

set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "src/")
file(GLOB_RECURSE SRC "src/*.cpp")
add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} PRIVATE
  Vulkan::Vulkan
  glfw3
  volk
  GPUOpen::VulkanMemoryAllocator
  glm::glm
  tinyobjloader::tinyobjloader
  ${Slang_LIBRARY}
  ${SHADERC_LIBRARY}
  ktx
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)

# Shader compilation
find_program(GLSLC glslc 
  /usr/local/bin/
  /usr/bin/
)
if(NOT GLSLC)
  message(FATAL_ERROR "Failed to locate glslc on the system")
endif()

set(SHADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
set(SPIRV_DIR "${CMAKE_BINARY_DIR}/shaders")
file(MAKE_DIRECTORY ${SPIRV_DIR})

file(GLOB_RECURSE SHADERS_SRC 
  "${SHADER_DIR}/*.vert"
  "${SHADER_DIR}/*.frag"
)
set(SPIRV_FILES "")

foreach(SHADER ${SHADERS_SRC})
  get_filename_component(SHADER_NAME ${SHADER} NAME)
  set(SPIRV_FILE ${SPIRV_DIR}/${SHADER_NAME}.spv)
  add_custom_command(
    OUTPUT ${SPIRV_FILE}
    COMMAND ${GLSLC} ${SHADER} -o ${SPIRV_FILE}
    DEPENDS ${SHADER}
    COMMENT "Compiling shader: ${SHADER_NAME} -> ${SPIRV_FILE}"
  )
  list(APPEND SPIRV_FILES ${SPIRV_FILE})
endforeach()
add_custom_target(CompileShaders ALL DEPENDS ${SPIRV_FILES})
add_dependencies(${PROJECT_NAME} CompileShaders)
