cmake_minimum_required(VERSION 3.8)
project(OpenGLTemplate LANGUAGES CXX)

set(OpenGL_GL_PREFERENCE GLVND)
set(CMAKE_CXX_STANDARD 14)

set(GLAD_SRC vendor/glad/glad.cpp 
             vendor/glad/glad.h)
set(HELPERS_SRC src/helpers/shader.hpp
                src/helpers/object.hpp
                src/helpers/objloader.cpp src/helpers/objloader.hpp 
                vendor/objLoader/OBJ_Loader.h)

# Use this insted of target_include_directories, because this is global
include_directories(src vendor)

# Glad
add_library(glad STATIC ${GLAD_SRC})

find_package(glfw3 3.2 REQUIRED)
find_package(OpenGL REQUIRED)

set(ALL_LIBS OpenGL::GL glfw glad dl)

add_executable(example src/example.cpp ${HELPERS_SRC})
target_link_libraries(example PUBLIC  ${ALL_LIBS})

add_executable(objLoad src/objLoad.cpp ${HELPERS_SRC})
target_link_libraries(objLoad PUBLIC  ${ALL_LIBS})

add_executable(lab1 src/lab1.cpp ${HELPERS_SRC})
target_link_libraries(lab1 PUBLIC  ${ALL_LIBS})

add_executable(lab2 src/lab2.cpp ${HELPERS_SRC})
target_link_libraries(lab2 PUBLIC  ${ALL_LIBS})

add_executable(lab2_1 src/lab2_1.cpp ${HELPERS_SRC})
target_link_libraries(lab2_1 PUBLIC  ${ALL_LIBS})

add_executable(crane src/crane.cpp ${HELPERS_SRC})
target_link_libraries(crane PUBLIC  ${ALL_LIBS})

add_executable(lab3 src/lab3.cpp ${HELPERS_SRC})
target_link_libraries(lab3 PUBLIC  ${ALL_LIBS})

add_executable(lab3_1 src/lab3_1.cpp ${HELPERS_SRC})
target_link_libraries(lab3_1 PUBLIC  ${ALL_LIBS})