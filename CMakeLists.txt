cmake_minimum_required(VERSION 3.20)

project(CW LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(3DRender
    src/main.cpp
    src/Bezier.cpp
    src/camera.cpp
    src/DayNightCycle.cpp
    src/Light.cpp
    src/LightManager.cpp
    src/Mesh.cpp
    src/Model.cpp
    src/ParticleEmitter.cpp
    src/Player.cpp
    src/Procedual.cpp
    src/Scene.cpp
    src/SceneNode.cpp
    src/Skybox.cpp
    src/Texture.cpp

    # OpenGL function loader
    include/GL/gl3w.c
)

target_include_directories(3DRender PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(3DRender PRIVATE
    glfw
    OpenGL::GL
)
# Copy runtime assets next to the executable
add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Objects
        $<TARGET_FILE_DIR:3DRender>/Objects

    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Particles
        $<TARGET_FILE_DIR:3DRender>/Particles

    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/src/pbr.vert
        $<TARGET_FILE_DIR:3DRender>/pbr.vert

    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/src/pbr.frag
        $<TARGET_FILE_DIR:3DRender>/pbr.frag

    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/src/shadow.vert
        $<TARGET_FILE_DIR:3DRender>/shadow.vert

    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/src/shadow.frag
        $<TARGET_FILE_DIR:3DRender>/shadow.frag

    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/src/particle.vert
        $<TARGET_FILE_DIR:3DRender>/particle.vert

    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/src/particle.frag
        $<TARGET_FILE_DIR:3DRender>/particle.frag
)
