cmake_minimum_required(VERSION 3.8)
project(OpenGLTemplate LANGUAGES CXX)

set(OpenGL_GL_PREFERENCE GLVND)
set(CMAKE_CXX_STANDARD 14)

set(GLAD_SRC glad/glad.cpp glad/glad.h)
set(HELPERS_SRC helpers/shader.hpp)

# Use this insted of target_include_directories, because this is global
include_directories(glad helpers)


# Glad
add_library(glad STATIC ${GLAD_SRC})

find_package(glfw3 3.2 REQUIRED)
find_package(OpenGL REQUIRED)

set(ALL_LIBS OpenGL::GL glfw glad dl)

add_executable(example example.cpp ${HELPERS_SRC})
target_link_libraries(example PUBLIC  ${ALL_LIBS})

add_executable(lab1 lab1.cpp ${HELPERS_SRC})
target_link_libraries(lab1 PUBLIC  ${ALL_LIBS})