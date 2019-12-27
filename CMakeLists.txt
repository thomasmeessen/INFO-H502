cmake_minimum_required (VERSION 3.15)
project ("INFO H-502 (ULB) - Thomas Meessen")



# OpenGL
find_package(OpenGL REQUIRED)
# GLM
find_package(glm REQUIRED)
# GLFW
find_package(glfw3 REQUIRED)

find_package(GLEW REQUIRED)

# ASSIMP
cmake_policy(SET CMP0012 OLD)
find_package(assimp REQUIRED)
#message(STATUS "Found(${assimp_FOUND}) ASSIMP ${assimp_FIND_VERSION}")
cmake_policy(SET CMP0012 NEW)
#find_package(FreeGLUT REQUIRED)

include(ExternalProject)
include( ${CMAKE_CURRENT_SOURCE_DIR}/External-stbImage.cmake)


include_directories("${PROJECT_SOURCE_DIR}/Include" "${PROJECT_SOURCE_DIR}/vendors/stb")


set(SRC_FILES
        Sources/main.cpp
        Sources/Shader.cpp
        Sources/skybox.cpp
        Sources/shadow.cpp
        Sources/thruster_particles.cpp
        Sources/weather_particles.cpp
        )
add_executable(project ${SRC_FILES})


add_dependencies(project OpenGL::OpenGL stbImage)

target_link_libraries(project OpenGL ${CMAKE_DL_LIBS} glfw glm GL GLEW assimp)
