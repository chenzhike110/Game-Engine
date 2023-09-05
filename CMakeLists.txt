cmake_minimum_required(VERSION 3.17)
project(GamEngine)

set(CMAKE_CXX_STANDARD 17)

#opengl
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS})

# imgui
add_subdirectory(extern/glfw)
add_subdirectory(extern/imgui)
set(IMGUI_DIR ${PROJECT_SOURCE_DIR}/extern/imgui)
file(GLOB IMGUI_SRC 
    ${IMGUI_DIR}/*.cpp
    ${IMGUI_DIR}/*.h
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp 
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# visualization
file(GLOB VIS_SOURCE_FILES
    ${PROJECT_SOURCE_DIR}/Visualization/*.cpp
    ${PROJECT_SOURCE_DIR}/extern/glfw/deps/glad_gl.c
)

# pbd
set(PBD ${PROJECT_SOURCE_DIR}/libs/PositionBasedDynamics/lib)

add_executable(GamEngine main.cpp ${IMGUI_SRC} ${VIS_SOURCE_FILES})

include_directories(
    ${PROJECT_SOURCE_DIR}/libs/PositionBasedDynamics/include
    ${PROJECT_SOURCE_DIR}/libs/PositionBasedDynamics/include/Visualization
)

target_link_libraries(GamEngine
    ${PBD}/libPositionBasedDynamics.a
    ${PBD}/libSimulation.a
    ${PBD}/libUtils.a
    imgui
    glfw
    ${OPENGL_LIBRARIES}
)