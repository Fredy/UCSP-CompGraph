#include "glad.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <array>
#include <vector>
#include <cmath>
using namespace std;

const GLuint WIDTH = 800, HEIGHT = 600;
const int CIRCLE_VERTEX_AMOUNT = 600;
constexpr const float PI = 3.141592653589793f;

struct DataWrapper {
  GLuint vao;
  GLuint vbo;
};

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
void main() {
  FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
})";

void frameBufferSizeCallback(GLFWwindow* window, int width, int height) {
    cout << "Width and height: " << width << ", " << height << "\n";
    glViewport(0, 0, width, height);
}  

DataWrapper createData(GLsizei size, const GLvoid *data) {
  GLuint vbo, vao;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void *) 0);
  glEnableVertexAttribArray(0);

  // Unbinding the buffer because it is already registered in the VAO.
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  // This is optional, unbinding VAO.
  glBindVertexArray(0);

  return {vao, vbo}; 

}

DataWrapper createSquare(array<float, 2> center, float edge) {
  float bottomLeft[] = {center[0] - edge / 2, center[1] - edge / 2};
  GLfloat vertices[] = {
    bottomLeft[0], bottomLeft[1], 0.0f,
    bottomLeft[0], bottomLeft[1] + edge, 0.0f,
    bottomLeft[0] + edge, bottomLeft[1] + edge, 0.0f,
    bottomLeft[0] + edge, bottomLeft[1], 0.0f,
  };

  DataWrapper data = createData(sizeof(vertices), vertices);
  return data; 
}

vector<float> genCircleVertices(const array<float, 2> center, const float radius, const int vertexAmount) {
  vector<float> vertices(vertexAmount * 3);

  const float kSq = center[1] * center[1]; 
  const float rSq = radius * radius;
  const float stride = (radius * 2 / vertexAmount) * 2;
  const float h0 = center[0] - radius;

  for (int i = 0; i < vertexAmount / 2; i++) {
    float x = h0 + stride * i;
    float tmp = center[0] - x;
    float sqrtR = sqrt(4 * kSq - 4 * (kSq +  tmp * tmp - rSq)) / 2; 
    float yTop = center[1] + sqrtR;
    float yBot = center[1] - sqrtR;
    vertices[i * 3] = x;
    vertices[i * 3 + 1] = yTop;
    vertices[i * 3 + 2] = 0.0f;
    // Setting the mirror vertex.
    *(vertices.rbegin() + i * 3 + 2) = x;
    *(vertices.rbegin() + i * 3 + 1) = yBot;
    *(vertices.rbegin() + i * 3) = 0.0f;
  }
  return  vertices;
}

DataWrapper createCircle(const array<float, 2> center, const float radius) {
  vector<GLfloat> vertices = genCircleVertices(center, radius, CIRCLE_VERTEX_AMOUNT);

  DataWrapper data = createData(sizeof(GLfloat) * vertices.size(), &vertices[0]) ;
  return data;
}

// - center and radius ar the center and radius of the biggest circle,
// - reduction is the ratio that the radius of a circle will have compared to 
//   the radius of the previous circle, 
// - amount is the number of circles that will be drawn
DataWrapper createCirclesInsideCircle(const array<float, 2> center, float radius, const float reduction, const int amount) {
  vector<GLfloat> vertices = genCircleVertices(center, radius, CIRCLE_VERTEX_AMOUNT);

  const float x0 = center[0] - radius;
  for (int i = 1; i < amount; i++) {
    radius = radius * reduction;
    const float tmpCenter = x0 + radius; 
    vector<GLfloat> tmp = genCircleVertices({tmpCenter, center[1]}, radius, CIRCLE_VERTEX_AMOUNT);
    vertices.insert(vertices.end(), tmp.begin(), tmp.end());
  }

  DataWrapper data = createData(sizeof(GLfloat) * vertices.size(), &vertices[0]) ;
  return data;
}

// - center and radius ar the center and radius of the biggest circle,
// - reduction is the ratio that the radius of a circle will have compared to 
//   the radius of the previous circle, 
// - amount is the number of circles that will be drawn
DataWrapper createCirclesLine1(const array<float, 2> center, float radius, const float reduction, const int amount) {
  vector<GLfloat> vertices = genCircleVertices(center, radius, CIRCLE_VERTEX_AMOUNT);

  float lastX = center[0] + radius;

  for (int i = 1; i < amount; i++) {
    radius = radius * reduction;
    const float tmpCenter = lastX + radius; 
    lastX = tmpCenter + radius;
    vector<GLfloat> tmp = genCircleVertices({tmpCenter, center[1]}, radius, CIRCLE_VERTEX_AMOUNT);
    vertices.insert(vertices.begin() + vertices.size() / 2, tmp.begin(), tmp.end());
  }

  DataWrapper data = createData(sizeof(GLfloat) * vertices.size(), &vertices[0]);
  return data;
}

constexpr float degToRad(float angle) {
    return angle * PI / 180.0f;
}

// - center and radius ar the center and radius of the biggest circle,
// - angle is the inclination angle in degrees,
// - reduction is the ratio that the radius of a circle will have compared to 
//   the radius of the previous circle, 
// - amount is the number of circles that will be drawn
DataWrapper createCirclesLine2(const array<float, 2> center, float radius, const float angle, const float reduction, const int amount) {
  vector<GLfloat> vertices = genCircleVertices(center, radius, CIRCLE_VERTEX_AMOUNT);

  float hypotenuse = radius; // This will be the distance between the first center and the current center.

  for (int i = 1; i < amount; i++) {
    radius = radius * reduction;
    hypotenuse += radius;
    array<float, 2> currentCenter = {
      center[0] + cos(degToRad(angle)) *  hypotenuse,
      center[1] + sin(degToRad(angle)) *  hypotenuse,
    };
    vector<GLfloat> tmp = genCircleVertices(currentCenter, radius, CIRCLE_VERTEX_AMOUNT);
    //vertices.insert(vertices.begin() + vertices.size() / 2, tmp.begin(), tmp.end());
    vertices.insert(vertices.end(), tmp.begin(), tmp.end());

    hypotenuse += radius;
  }

  GLuint vbo, vao;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void *) 0);
  glEnableVertexAttribArray(0);

  // Unbinding the buffer because it is already registered in the VAO.
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  // This is optional, unbinding VAO.
  glBindVertexArray(0);

  return {vao, vbo}; 
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

  DataWrapper square = createSquare({-100.0f, -100.0f}, 200.0f); // 1
  DataWrapper circle = createCircle({-100.0f, -100.0f}, 400.0); // 2
  const int circlesAmount = 6;
  DataWrapper circlesIn = createCirclesInsideCircle({0.0f, 100.0f}, 400.0f, 0.8f, circlesAmount); // 3
  DataWrapper circlesLine1 = createCirclesLine1({-500.0f, 0.0f}, 200.0f, 0.8f, circlesAmount); // 4
  DataWrapper circlesLine2 = createCirclesLine2({-500.0f, 0.0f}, 150.0f, 25, 0.8f, circlesAmount); // 5

    // Main loop
  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.95f, 0.95f, 0.95f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glBindVertexArray(circlesLine1.vao);
    glDrawArrays(GL_LINE_LOOP, 0, CIRCLE_VERTEX_AMOUNT * circlesAmount);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // TODO: delete VAOs and VBOs;
  glDeleteVertexArrays(1, &square.vao);
  // glDeleteBuffers(1, &vbo);

  glfwTerminate();
}

/*
 * Use program
 * Bind VAO
 * Draw Array*/
// glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices2), vertices2);
