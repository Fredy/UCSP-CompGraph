#include "helpers/object.hpp"
#include "helpers/shader.hpp"
#include <GLFW/glfw3.h>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include <array>
using namespace std;

const GLuint WIDTH = 800, HEIGHT = 600;

vector<float> genCircleVertices(const array<float, 3> center, const float radius, const int vertexAmount) {
  vector<float> vertices(vertexAmount * 3 + 3 + 3);
  vertices[0] = center[0];
  vertices[1] = center[1];
  vertices[2] = center[2];

  for (int i = 0; i < vertexAmount; i++) {
    vertices[i * 3 + 3] = center[0] + radius * cos((i + 1.0f) * 2.0f * glm::pi<float>() / float(vertexAmount));
    vertices[i * 3 + 4] = center[1] + radius * sin((i + 1.0f) * 2.0f * glm::pi<float>() / float(vertexAmount));
    vertices[i * 3 + 5] = 0.0f;
  }
  vertices[vertices.size() - 1] = vertices[5];
  vertices[vertices.size() - 2] = vertices[4];
  vertices[vertices.size() - 3] = vertices[3];
  return  vertices;
}

void frameBufferSizeCallback(GLFWwindow *window, int width, int height) {
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
  glfwWindowHint(GLFW_SAMPLES, 4); // Anti aliasing
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

  GLFWwindow *window =
      glfwCreateWindow(WIDTH, HEIGHT, "Test window", nullptr, nullptr);
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

  //////////////////////////////////////////////////////////////////////

  // Build and compile shader programs.
  Shader shaders("shaders/vshader.glsl", "shaders/fshader.glsl");

  // Vertex data
  GLfloat vertices[] = {
      -17.3f, -10.0f, 0.0f, // left
      17.3f, -10.0f, 0.0f, // right
      0.0f,  20.0f, 0.0f, // top
  };

  GLfloat colors[] = {
      1.0f, 0.0f, 0.0f, // red
      0.0f, 1.0f, 0.0f, // green
      0.0f, 0.0f, 1.0f, // blue
  };

  GLfloat verticesGizmo[] = {
      0.0f, 0.0f, 0.0f, // X
      80.0f, 0.0f, 0.0f, //
      0.0f, 0.0f, 0.0f, // Y
      0.0f, 80.0f, 0.0f, //
      0.0f, 0.0f, 0.0f, // Z
      0.0f, 0.0f, 80.0f,
  };

  GLfloat colorsGizmo[] = {
      1.0f, 0.0f, 0.0f, // red
      1.0f, 0.0f, 0.0f, // red
      0.0f, 1.0f, 0.0f, // green
      0.0f, 1.0f, 0.0f, // green
      0.0f, 0.0f, 1.0f, // blue
      0.0f, 0.0f, 1.0f, // blue
  };

  GLfloat verticesSquare[] = {
    -5.0f, -5.0f, 0.0f, // bl
    5.0f, -5.0f, 0.0f, // br
    5.0f, 5.0f, 0.0f, // tr

    -5.0f, -5.0f, 0.0f, // bl
    5.0f, 5.0f, 0.0f, // tr
    -5.0f, 5.0f, 0.0f, // tl
  };

  GLfloat colorsSquare[] = {
      0.9f, 0.5f, 0.0f, // orange?
      0.9f, 0.5f, 0.0f, // orange?
      0.9f, 0.5f, 0.0f, // orange?
      0.9f, 0.5f, 0.0f, // orange?
      0.9f, 0.5f, 0.0f, // orange?
      0.9f, 0.5f, 0.0f, // orange?
  };

  GLfloat verticesTriangle2[] {
      -8.3f, -5.0f, 0.0f, // left
      8.3f, -5.0f, 0.0f, // right
      0.0f,  10.0f, 0.0f, // top
  };

  GLfloat colorsTriangle2[] = {
      0.43, 0.22, 0.627, // #0B38A0
      0.43, 0.22, 0.627, // #0B38A0
      0.43, 0.22, 0.627, // #0B38A0
  };

  const int circleVertices = 200;
  vector<float> verticesCircle = genCircleVertices({0.0f, 0.0f, 0.0f}, 8.0f, circleVertices);
  vector<float> colorsCircle(verticesCircle.size());

  colorsCircle[0] = 0.667;
  colorsCircle[1] = 0.224f;
  colorsCircle[2] = 0.224f;
  for (int i = 3 ; i < colorsCircle.size(); i += 3) {
    colorsCircle[i] = 0.949f;
    colorsCircle[i+1] = 0.949f;
    colorsCircle[i+2] = 0.0f;
  }


  Object triangle;
  triangle.setVerticesData(3, vertices, colors);
  triangle.setMode(GL_TRIANGLES);
  triangle.setShader(shaders.ID);

  //Shader simpleShader("shaders/minimal_v.glsl", "shaders/minimal_f.glsl");
  Object gizmo;
  gizmo.setVerticesData(6, verticesGizmo, colorsGizmo);
  gizmo.setMode(GL_LINES);
  gizmo.setShader(shaders.ID);

  Object square;
  square.setVerticesData(6, verticesSquare, colorsSquare);
  square.setMode(GL_TRIANGLES);
  square.setShader(shaders.ID);

  
  Shader invShader("shaders/lab2_inv_v.glsl", "shaders/lab2_inv_f.glsl");
  Object circle;
  circle.setVerticesData(circleVertices + 2, &verticesCircle[0], &colorsCircle[0]);
  circle.setMode(GL_TRIANGLE_FAN);
  circle.setShader(invShader.ID);

  Object triangle2;
  triangle2.setVerticesData(3, verticesTriangle2, colorsTriangle2);
  triangle2.setMode(GL_TRIANGLES);
  triangle2.setShader(invShader.ID);

  // MVP
   glm::mat4 projectionMat = glm::perspective(glm::radians(90.0f),
   float(WIDTH) / HEIGHT, 0.1f, 250.0f);
  // For ortho camera:
  // glm::mat4 projectionMat = glm::ortho(-400.0f, 400.0f, -300.0f, 300.0f, 0.0f,
                                      //  100.0f); // In world coordinates

  glm::mat4 viewMat = glm::lookAt(glm::vec3{90, 80, 90}, {0, 0, 0}, {0, 1, 0});

  glm::mat4 modelMat = glm::mat4(1.0f);
  //modelMat = glm::scale(modelMat, {0.2f, 0.2f, 0.2f});

  glm::mat4 mvp = projectionMat * viewMat * modelMat;
  GLuint matrixId = glGetUniformLocation(shaders.ID, "mvp");

  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  // Main loop
  float i = 0.0f;
  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);

    auto modelTriangle = [&] {
     // modelMat = glm::rotate(modelMat, glm::radians(i), {0.0f, 0.0f, 1.0f});
      modelMat = glm::translate(modelMat, {sin(i / 100.0f) * 80.0f, 0.0f, 0.0f});
    };

    triangle.draw([&] {
      modelMat = glm::mat4(1.0f);
      modelTriangle();
      mvp = projectionMat * viewMat * modelMat;
      glUniformMatrix4fv(matrixId, 1, GL_FALSE, &mvp[0][0]);
    });

    //  XYZ
    gizmo.draw([&] {
      modelMat = glm::mat4(1.0f);
      mvp = projectionMat * viewMat * modelMat;
      glUniformMatrix4fv(matrixId, 1, GL_FALSE, &mvp[0][0]);
    });

    auto modelSquare = [&] {
      modelTriangle();
      modelMat = glm::rotate(modelMat, glm::radians(i), {0.0f, 0.0f, 1.0f});
      modelMat = glm::translate(modelMat, {30.0f, 0.0f, 0.0f});
    };

    square.draw([&] {
      modelMat = glm::mat4(1.0f);
      modelSquare();
      mvp = projectionMat * viewMat * modelMat;
      glUniformMatrix4fv(matrixId, 1, GL_FALSE, &mvp[0][0]);
    });

    circle.draw([&] {
      modelMat = glm::mat4(1.0f);
      modelSquare();
      modelMat = glm::rotate(modelMat, glm::radians(i * 3.0f), {0.0f, 1.0f, 0.0f});
      modelMat = glm::translate(modelMat, {20.0f, 0.0f, 0.0f});
      mvp = projectionMat * viewMat * modelMat;
      glUniformMatrix4fv(matrixId, 1, GL_FALSE, &mvp[0][0]);
    });

    triangle2.draw([&]{
      modelMat = glm::mat4(1.0f);
      modelTriangle();
      modelMat = glm::rotate(modelMat, glm::radians(i * 5.0f), {1.0f, 0.0f, 0.0f});
      modelMat = glm::translate(modelMat, {0.0f, 30.0f, 0.0f});
      mvp = projectionMat * viewMat * modelMat;
      glUniformMatrix4fv(matrixId, 1, GL_FALSE, &mvp[0][0]);
    });

    glfwSwapBuffers(window);
    glfwPollEvents();
    i += 1.0f;
  }

  triangle.destroy();
  gizmo.destroy();
  square.destroy();
  circle.destroy();
  triangle2.destroy();

  glfwTerminate();
}

/*
 * Use program
 * Bind VAO
 * Draw Array*/
// glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices2), vertices2);
