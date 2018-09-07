#include "glad/glad.h"
#include "helpers/shader.hpp"
#include <GLFW/glfw3.h>
#include <iostream>
#include <array>
#include <vector>
#include <cmath>
using namespace std;

const GLuint WIDTH = 800, HEIGHT = 600;
const int CIRCLE_VERTEX_AMOUNT = 200;
constexpr const float PI = 3.141592653589793f;

struct DataWrapper {
  GLuint vao;
  GLuint vbo;
};

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

vector<DataWrapper> createSquare(array<float, 2> center, float edge) {
  float bottomLeft[] = {center[0] - edge / 2, center[1] - edge / 2};
  GLfloat vertices[] = {
    bottomLeft[0], bottomLeft[1], 0.0f,
    bottomLeft[0], bottomLeft[1] + edge, 0.0f,
    bottomLeft[0] + edge, bottomLeft[1] + edge, 0.0f,
    bottomLeft[0] + edge, bottomLeft[1], 0.0f,
  };

  DataWrapper data = createData(sizeof(vertices), vertices);
  return {data}; 
}

vector<float> genCircleVertices(const array<float, 2> center, const float radius, const int vertexAmount) {
  vector<float> vertices(vertexAmount * 3);

  for (int i = 0; i < vertexAmount; i++) {
    vertices[i * 3] = center[0] + radius * cos((i + 1.0f) * 2.0f * PI / float(vertexAmount));
    vertices[i * 3 + 1] = center[1] + radius * sin((i + 1.0f) * 2.0f * PI / float(vertexAmount));
    vertices[i * 3 + 2] = 0.0f;
  }
  return  vertices;
}

vector<DataWrapper> createCircle(const array<float, 2> center, const float radius) {
  vector<GLfloat> vertices = genCircleVertices(center, radius, CIRCLE_VERTEX_AMOUNT);

  DataWrapper data = createData(sizeof(GLfloat) * vertices.size(), &vertices[0]) ;
  return {data};
}

// - center and radius are the center and radius of the biggest circle,
// - reduction is the ratio that the radius of a circle will have compared to 
//   the radius of the previous circle, 
// - amount is the number of circles that will be drawn
vector<DataWrapper> createCirclesInsideCircle(const array<float, 2> center, float radius, const float reduction, const int amount) {
  vector<DataWrapper> data(amount);
  vector<GLfloat> vertices = genCircleVertices(center, radius, CIRCLE_VERTEX_AMOUNT);

  data[0] = createData(sizeof(GLfloat) * vertices.size(), &vertices[0]) ;
  
  const float x0 = center[0] - radius;
  for (int i = 1; i < amount; i++) {
    radius = radius * reduction;
    const float tmpCenter = x0 + radius; 
    vertices = genCircleVertices({tmpCenter, center[1]}, radius, CIRCLE_VERTEX_AMOUNT);
    data[i] = createData(sizeof(GLfloat) * vertices.size(), &vertices[0]) ;
  }

  return data;
}

// - center and radius are the center and radius of the biggest circle,
// - reduction is the ratio that the radius of a circle will have compared to 
//   the radius of the previous circle, 
// - amount is the number of circles that will be drawn
vector<DataWrapper> createCirclesLine1(const array<float, 2> center, float radius, const float reduction, const int amount) {
  vector<DataWrapper> data(amount);
  vector<GLfloat> vertices = genCircleVertices(center, radius, CIRCLE_VERTEX_AMOUNT);
  data[0] = createData(sizeof(GLfloat) * vertices.size(), &vertices[0]) ;

  float lastX = center[0] + radius;

  for (int i = 1; i < amount; i++) {
    radius = radius * reduction;
    const float tmpCenter = lastX + radius; 
    lastX = tmpCenter + radius;
    vertices = genCircleVertices({tmpCenter, center[1]}, radius, CIRCLE_VERTEX_AMOUNT);
    data[i] = createData(sizeof(GLfloat) * vertices.size(), &vertices[0]) ;
  }

  return data;
}

constexpr float degToRad(float angle) {
    return angle * PI / 180.0f;
}

// - center and radius are the center and radius of the biggest circle,
// - angle is the inclination angle in degrees,
// - reduction is the ratio that the radius of a circle will have compared to 
//   the radius of the previous circle, 
// - amount is the number of circles that will be drawn
vector<DataWrapper> createCirclesLine2(const array<float, 2> center, float radius, const float angle, const float reduction, const int amount) {
  vector<DataWrapper> data(amount);
  vector<GLfloat> vertices = genCircleVertices(center, radius, CIRCLE_VERTEX_AMOUNT);
  data[0] = createData(sizeof(GLfloat) * vertices.size(), &vertices[0]) ;

  float hypotenuse = radius; // This will be the distance between the first center and the current center.

  for (int i = 1; i < amount; i++) {
    radius = radius * reduction;
    hypotenuse += radius;
    array<float, 2> currentCenter = {
      center[0] + cos(degToRad(angle)) *  hypotenuse,
      center[1] + sin(degToRad(angle)) *  hypotenuse,
    };
    vertices = genCircleVertices(currentCenter, radius, CIRCLE_VERTEX_AMOUNT);
    data[i] = createData(sizeof(GLfloat) * vertices.size(), &vertices[0]) ;

    hypotenuse += radius;
  }

  return data;
}



int main() {
  // GLFW initialization
  if (!glfwInit()) {
    cout << "GLFW initialization failed\n";
    glfwTerminate();
    return 1;
  }

  // GLFW setup
  glfwWindowHint(GLFW_SAMPLES, 4); // Anti aliasing
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
  Shader shaders("shaders/lab1_v.glsl", "shaders/lab1_f.glsl");

  // // // // // // // // // // // 
  vector<DataWrapper> dataVec ;
  // dataVec = createSquare({-100.0f, -100.0f}, 200.0f); // 1 square
  // dataVec = createCircle({100.0f, -100.0f}, 400.0); // 2 circle
  // dataVec = createCirclesInsideCircle({0.0f, 100.0f}, 400.0f, 0.8f, 6); // 3 circles inside circle
  // dataVec = createCirclesLine1({-500.0f, 0.0f}, 200.0f, 0.8f, 6); // 4 circle line
  dataVec = createCirclesLine2({-500.0f, 0.0f}, 150.0f, 25, 0.8f, 6); // 5 circle line with inclination

  // Main loop
  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.95f, 0.95f, 0.95f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    shaders.use();
    for (const auto &data : dataVec) {
      glBindVertexArray(data.vao);
      glDrawArrays(GL_LINE_LOOP, 0, CIRCLE_VERTEX_AMOUNT); // TODO: change this so it works with the square too
    }
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  for (const auto &data : dataVec) {
    glDeleteVertexArrays(1, &data.vao);
    glDeleteBuffers(1, &data.vbo);
  }

  glfwTerminate();
}

/*
 * Use program
 * Bind VAO
 * Draw Array*/
// glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices2), vertices2);
