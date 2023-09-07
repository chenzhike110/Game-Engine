cmake_minimum_required(VERSION 3.17)
project(GamEngine)

# set(CMAKE_CXX_STANDARD 17)

set(PROJECT_PATH ${PROJECT_SOURCE_DIR})
include(${PROJECT_PATH}/CMake/Common.cmake)
include(${PROJECT_PATH}/CMake/DataCopyTargets.cmake)

# external projects
set(ExternalInstallDir "${PROJECT_BINARY_DIR}" CACHE INTERNAL "External install directory")	
include(ExternalProject)

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

add_subdirectory(extern/md5)

# visualization
file(GLOB VIS_SOURCE_FILES
    ${PROJECT_SOURCE_DIR}/visual/*.cpp
    ${PROJECT_SOURCE_DIR}/visual/*.h
    ${PROJECT_SOURCE_DIR}/extern/glfw/deps/glad_gl.c
)

# pbd physics
set(PBDLIB ${PROJECT_SOURCE_DIR}/physics)
add_subdirectory(physics)

ExternalProject_Add(
    Ext_assimp
    PREFIX "${CMAKE_BINARY_DIR}/extern/Assimp"
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    INSTALL_DIR ${ExternalInstallDir}/assimp
    CMAKE_ARGS -DCMAKE_BUILD_TYPE:STRING=${EXT_CMAKE_BUILD_TYPE} -DCMAKE_INSTALL_PREFIX:PATH=${ExternalInstallDir}/assimp
    -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS} -DASSIMP_BUILD_TESTS=OFF
) 
ExternalProject_Get_Property(Ext_assimp INSTALL_DIR)
set(Assimp_INCLUDE_DIR ${INSTALL_DIR}/include)
set(Assimp_LIBRARIES optimized ${INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}assimp${CMAKE_SHARED_LIBRARY_SUFFIX} 
                     debug ${INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}assimp_d${CMAKE_SHARED_LIBRARY_SUFFIX})
unset(INSTALL_DIR)
message(STATUS "Building ${assimp_INCLUDE_DIR}")

include_directories(
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/physics
)

add_executable(GamEngine main.cpp ${IMGUI_SRC} ${VIS_SOURCE_FILES})

set(SIMULATION_LINK_LIBRARIES imgui glfw PositionBasedDynamics Simulation Utils)
set(SIMULATION_DEPENDENCIES imgui glfw PositionBasedDynamics Simulation Utils CopyPBDShaders CopyImguiFonts CopyPBDModels)

add_dependencies(GamEngine ${SIMULATION_DEPENDENCIES})

target_link_libraries(GamEngine
    ${SIMULATION_LINK_LIBRARIES}
    ${OPENGL_LIBRARIES}
)