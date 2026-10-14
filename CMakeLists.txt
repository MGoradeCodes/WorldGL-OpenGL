
cmake_minimum_required(VERSION 3.20)

project(Project1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(libs/glfw)

add_executable(Project1
    src/main.cpp
    libs/glad/src/gl.c
    src/VAO.cpp
    src/VBO.cpp
    src/EBO.cpp
    src/ShaderClass.cpp
    src/Camera.cpp
    src/Mesh.cpp
    src/stb.cpp
    src/Texture.cpp
    src/World.cpp
    src/Chunk.cpp
)

target_compile_definitions(Project1 PRIVATE GLM_ENABLE_EXPERIMENTAL)
target_include_directories(Project1 PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/glad/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/glm"
)

target_link_libraries(Project1 PRIVATE
    glfw
    opengl32
)

set(SHADER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/shaders")
set(TEXTURE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/textures")

add_custom_command(TARGET Project1 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${SHADER_SOURCE_DIR}"
        "$<TARGET_FILE_DIR:Project1>/shaders"

    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${TEXTURE_SOURCE_DIR}"
        "$<TARGET_FILE_DIR:Project1>/textures"

    COMMENT "Copying shaders and textures..."
    VERBATIM
)
