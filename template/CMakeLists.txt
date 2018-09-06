cmake_minimum_required(VERSION 3.8)
project(OpenGLTemplate LANGUAGES CXX)

set(OpenGL_GL_PREFERENCE GLVND)
set(CMAKE_CXX_STANDARD 14)

set(GLAD_SRC glad/glad.cpp glad/glad.h)
set(PROJECT_SRC main.cpp helpers/shader.hpp)

add_executable(project ${PROJECT_SRC})
target_include_directories(project PRIVATE glad helpers)

# Glad
add_library(glad STATIC ${GLAD_SRC})

find_package(glfw3 3.2 REQUIRED)
find_package(OpenGL REQUIRED)

target_link_libraries(project PUBLIC OpenGL::GL glfw glad dl)