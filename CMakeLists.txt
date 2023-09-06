cmake_minimum_required(VERSION 3.17)
project(GamEngine)

# set(CMAKE_CXX_STANDARD 17)

set(PROJECT_PATH ${PROJECT_SOURCE_DIR})
include(${PROJECT_PATH}/CMake/Common.cmake)
include(${PROJECT_PATH}/CMake/DataCopyTargets.cmake)

# find_package(Eigen3 REQUIRED)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
# include_directories( ${EIGEN3_INCLUDE_DIR} )

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

# pbd
set(PBDLIB ${PROJECT_SOURCE_DIR}/PBD)
# set(GenericParameters_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/libs/GenericParameters/include)
# set(Discregrid_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/libs/Discregrid/include)
# set(DiscregridLIB ${PROJECT_SOURCE_DIR}/libs/Discregrid/lib)
add_subdirectory(PBD)

add_executable(GamEngine main.cpp ${IMGUI_SRC} ${VIS_SOURCE_FILES})

include_directories(
    # ${PROJECT_SOURCE_DIR}/libs/PositionBasedDynamics
    # ${GenericParameters_INCLUDE_DIR}
    # ${Discregrid_INCLUDE_DIR}
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/PBD
)

set(SIMULATION_LINK_LIBRARIES imgui glfw PositionBasedDynamics Simulation Utils)
set(SIMULATION_DEPENDENCIES imgui glfw PositionBasedDynamics Simulation Utils CopyPBDShaders CopyImguiFonts CopyPBDModels)

add_dependencies(GamEngine ${SIMULATION_DEPENDENCIES})

target_link_libraries(GamEngine
    ${SIMULATION_LINK_LIBRARIES}
    ${OPENGL_LIBRARIES}
)