cmake_minimum_required(VERSION 3.8)
project(OpenGLTemplate LANGUAGES C CXX)

set(OpenGL_GL_PREFERENCE GLVND)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD 11)

set(GLAD_SRC vendor/glad/glad.c 
             vendor/glad/glad.h)
set(HELPERS_SRC src/helpers/camera.cpp src/helpers/camera.hpp
                vendor/objLoader/OBJ_Loader.h)

# Use this insted of target_include_directories, because this is global
include_directories(src vendor)

# Glad
add_library(glad STATIC ${GLAD_SRC})

find_package(glfw3 3.2 REQUIRED)
find_package(OpenGL REQUIRED)

set(ALL_LIBS OpenGL::GL glfw glad dl)

add_executable(objLoad src/objLoad.cpp ${HELPERS_SRC})
target_link_libraries(objLoad PUBLIC  ${ALL_LIBS})

add_executable(lab4 src/lab4.cpp ${HELPERS_SRC})
target_link_libraries(lab4 PUBLIC  ${ALL_LIBS})