#include "glad.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
using namespace std;

const GLuint WIDTH = 800, HEIGHT = 600;

const char *vertexShaderSrc = R"(
#version 330 core

layout (location = 0) in vec3 aPos;
uniform vec2 windowSzie;
void main() {
  gl_Position = vec4(aPos.x / 800.0f , aPos.y / 600.0f, aPos.z, 1.0);
})";

const char * fragmentShaderSrc = R"(
#version 330 core

out vec4 FragColor;
uniform vec4 outColor;

void main() {
  FragColor = outColor;
})";

void frameBufferSizeCallback(GLFWwindow* window, int width, int height) {
    cout << "Width and height: " << width << ", " << height << "\n";
    glViewport(0, 0, width, height);
}  

int main() {
  // GLFW initialization
  if (!glfwInit()) {
    cout << "GLFW initialization failed\n";
    glfwTerminate();
    return 1;
  }

  // GLFW setup
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

  GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Test window", nullptr, nullptr);
  if (!window) {
    cout << "GLFW windows creation failed failed\n";
    glfwTerminate();
    return 1;
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);  

  // Glad initialization
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    cout << "Failed to initialize GLAD\n";
    return 1;
  }

  // Build and compile shader programs.
  // Vertex shader.
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSrc, nullptr);
  glCompileShader(vertexShader);
  // Check compilation errors
  GLint success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
    cout << "ERROR: Vertex shader compilation failed:\n" << infoLog << '\n';
  }

  // Fragment shader.
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSrc, nullptr);
  glCompileShader(fragmentShader);
  // Check compilation errors.
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
    cout << "ERROR: Fragment shader compilation failed:\n" << infoLog << '\n';
  }

  // Link shaders.
  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  // Check link errors.
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
    cout << "ERROR: Program linking failed:\n" << infoLog << '\n';
  }
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  // Vertex data 
  GLfloat vertices[] = {
    /*
    -0.5f, -0.5f, 0.0f, // left  
    0.5f, -0.5f, 0.0f, // right 
    0.5f,  0.5f, 0.0f,  // top  
    -0.5f, 0.5f, 0.0f
    */
    -200.0f, -200.0f, 0.0f,
    200.0f, -200.0f, 0.0f,
    200.0f, 200.0f, 0.0f,
    -200.0f, 200.0f, 0.0f,
  };

 GLfloat vertices2[] = {
    -100.0f, -100.0f, 0.0f,
    100.0f, -100.0f, 0.0f,
    100.0f, 100.0f, 0.0f,
    -100.0f, 100.0f, 0.0f
  };



  GLuint vbo, vao, vbo2;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void *) 0);
  glEnableVertexAttribArray(0);

  // Unbinding the buffer because it is already registered in the VAO.
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  // This is optional, unbinding VAO.
  //glBindVertexArray(0);

  // Main loop
  while (!glfwWindowShouldClose(window)) {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    //glBindVertexArray(vao);

    float greenValue = sin(glfwGetTime());
    GLint vertexColorLocation = glGetUniformLocation(shaderProgram, "outColor");
    glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

    glDrawArrays(GL_TRIANGLES, 0, 4);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);

  glfwTerminate();
}

/*
 * Use program
 * Bind VAO
 * Draw Array*/
// glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices2), vertices2);
