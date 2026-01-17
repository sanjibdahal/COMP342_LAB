cmake_minimum_required(VERSION 3.10)
project(GravitySimulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Find GLFW
find_package(glfw3 REQUIRED)

# Find GLEW
find_package(GLEW REQUIRED)

# Find GLM
find_package(glm REQUIRED)

# Include directories
include_directories(
    ${OPENGL_INCLUDE_DIRS}
    ${GLFW_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/libs
    ${CMAKE_SOURCE_DIR}/src
)

# Source files
set(SOURCES
    src/main.cpp
    src/Body.cpp
    src/PhysicsEngine.cpp
    src/Camera.cpp
    src/Shader.cpp
    src/Renderer.cpp
    src/UI.cpp
)

# Create executable
add_executable(GravitySimulator ${SOURCES})

# Link libraries
target_link_libraries(GravitySimulator
    ${OPENGL_LIBRARIES}
    glfw
    GLEW::GLEW
    ${CMAKE_DL_LIBS}
)

# For Linux, add math library
if(UNIX AND NOT APPLE)
    target_link_libraries(GravitySimulator m)
endif()

# Copy shaders to build directory (if using external shader files)
# file(COPY shaders DESTINATION ${CMAKE_BINARY_DIR})

# Print configuration
message(STATUS "OpenGL: ${OPENGL_LIBRARIES}")
message(STATUS "GLFW: ${GLFW_LIBRARIES}")
message(STATUS "GLEW: ${GLEW_LIBRARIES}")
message(STATUS "GLM: ${GLM_INCLUDE_DIRS}")