cmake_minimum_required(VERSION 3.10)
project(GravitySimulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenGL
find_package(OpenGL REQUIRED)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glfw)

# GLAD
add_library(glad external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)

# GLM
add_subdirectory(external/glm)

# ImGui
set(IMGUI_DIR external/imgui)
add_library(imgui
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC 
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)
target_link_libraries(imgui glfw ${OPENGL_LIBRARIES})

# Main executable
add_executable(GravSim src/Main.cpp)

target_include_directories(GravSim PRIVATE
    external/glad/include
    external/glm
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

target_link_libraries(GravSim
    glad
    glfw
    glm
    imgui
    ${OPENGL_LIBRARIES}
)

if(WIN32)
    target_link_libraries(GravSim opengl32)
endif()