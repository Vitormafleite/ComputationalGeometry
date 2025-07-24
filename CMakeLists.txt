cmake_minimum_required(VERSION 3.10)
project(GeometricModeling)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

cmake_policy(SET CMP0072 NEW)  # Use GLVND
set(OpenGL_GL_PREFERENCE GLVND)



# Find packages
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# Include paths
include_directories(
    headers
    external/glad/include
    external/imgui
    external/imgui/backends
    external/glm
)

# Core ImGui source files (only the needed .cpp)
set(IMGUI_CORE
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/imgui_demo.cpp
)

# ImGui backends for GLFW and OpenGL3 only
set(IMGUI_BACKENDS
    external/imgui/backends/imgui_impl_glfw.cpp
    external/imgui/backends/imgui_impl_opengl3.cpp
)

# Your project .cpp files
file(GLOB PROJECT_SOURCES CONFIGURE_DEPENDS
    source/*.cpp
    main.cpp
)

# Final executable
add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCES}
    external/glad/glad.c
    ${IMGUI_CORE}
    ${IMGUI_BACKENDS}
)

# Link OpenGL and GLFW
target_link_libraries(${PROJECT_NAME}
    OpenGL::GL
    glfw
)
