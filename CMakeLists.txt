cmake_minimum_required(VERSION 3.17)
project(GamEng)

include(${PROJECT_SOURCE_DIR}/CMake/Common.cmake)

set(ExternalInstallDir "${PROJECT_BINARY_DIR}" CACHE INTERNAL "External install directory")	
include(ExternalProject)

#assimp
ExternalProject_Add(
    Ext_assimp
    PREFIX "${CMAKE_BINARY_DIR}/extern/assimp"
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    INSTALL_DIR ${ExternalInstallDir}/assimp
    CMAKE_ARGS -DCMAKE_BUILD_TYPE:STRING=${EXT_CMAKE_BUILD_TYPE} -DCMAKE_INSTALL_PREFIX:PATH=${ExternalInstallDir}/assimp
    -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS} -DASSIMP_BUILD_TESTS=OFF
) 
ExternalProject_Get_Property(Ext_assimp INSTALL_DIR)
set(Assimp_INCLUDE_DIR ${INSTALL_DIR}/include ${PROJECT_SOURCE_DIR}/extern/utf8cpp)
set(Assimp_LIBRARIES optimized ${INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}assimp${CMAKE_SHARED_LIBRARY_SUFFIX} 
                     debug ${INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}assimp_d${CMAKE_SHARED_LIBRARY_SUFFIX})
unset(INSTALL_DIR)
message(STATUS "Building ${assimp_INCLUDE_DIR}")

#opengl
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS})

# imgui
add_subdirectory(extern/glfw)
add_subdirectory(extern/imgui)
set(IMGUI_DIR ${PROJECT_SOURCE_DIR}/extern/imgui)

#render
file(GLOB VIS_SRC 
    ${PROJECT_SOURCE_DIR}/render/*.cpp
    ${PROJECT_SOURCE_DIR}/render/*.h
    ${PROJECT_SOURCE_DIR}/extern/glfw/deps/glad_gl.c
)

#utils
file(GLOB UTIL_SRC
    ${PROJECT_SOURCE_DIR}/utils/*.cpp
    ${PROJECT_SOURCE_DIR}/utils/*.h
)

include_directories(
    ${IMGUI_DIR}
    ${PROJECT_SOURCE_DIR}
    ${Assimp_INCLUDE_DIR}
)

set(RENDER_LINK_LIBRARIES imgui glfw ${OPENGL_LIBRARIES})

file(COPY ${PROJECT_SOURCE_DIR}/data/ DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources)

add_executable(GamEng main.cpp ${VIS_SRC} ${UTIL_SRC})
target_link_libraries(GamEng
    ${RENDER_LINK_LIBRARIES}
    ${Assimp_LIBRARIES}
)
