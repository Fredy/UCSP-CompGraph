#include "helpers/object.hpp"
#include "helpers/shader.hpp"
#include <GLFW/glfw3.h>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
using namespace std;

const GLuint WIDTH = 800, HEIGHT = 600;

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
      -43.3f, -25.0f, 0.0f, // left
      43.3f, -25.0f, 0.0f, // right
      0.0f,  50.0f, 0.0f, // top
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

  Object triangle;
  triangle.setVerticesData(3, vertices, colors);
  triangle.setMode(GL_TRIANGLES);
  triangle.setShader(shaders.ID);

  Shader simpleShader("shaders/minimal_v.glsl", "shaders/minimal_f.glsl");
  Object gizmo;
  gizmo.setVerticesData(6, verticesGizmo, colorsGizmo);
  gizmo.setMode(GL_LINES);
  gizmo.setShader(shaders.ID);

  // MVP
  // glm::mat4 projectionMat = glm::perspective(glm::radians(45.0f),
  // float(WIDTH) / HEIGHT, 0.1f, 100.0f);
  // For ortho camera:
  glm::mat4 projectionMat = glm::ortho(-400.0f, 400.0f, -300.0f, 300.0f, 0.0f,
                                       100.0f); // In world coordinates

  glm::mat4 viewMat = glm::lookAt(glm::vec3{0, 0, 1}, {0, 0, 0}, {0, 1, 0});

  glm::mat4 modelMat = glm::mat4(1.0f);
  //modelMat = glm::scale(modelMat, {0.2f, 0.2f, 0.2f});

  glm::mat4 mvp = projectionMat * viewMat * modelMat;
  GLuint matrixId = glGetUniformLocation(shaders.ID, "mvp");

  // Main loop
  float i = 0.0f;
  while (!glfwWindowShouldClose(window)) {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    triangle.draw([&] {
      modelMat = glm::mat4(1.0f);
      modelMat = glm::rotate(modelMat, glm::radians(i), {0.0f, 0.0f, 1.0f});
      modelMat = glm::translate(modelMat, {80.0f, 0.0f, 0.0f});
      mvp = projectionMat * viewMat * modelMat;
      glUniformMatrix4fv(matrixId, 1, GL_FALSE, &mvp[0][0]);
    });

    //  XYZ
    gizmo.draw([&] {
      modelMat = glm::mat4(1.0f);

      mvp = projectionMat * viewMat * modelMat;
      glUniformMatrix4fv(matrixId, 1, GL_FALSE, &mvp[0][0]);
    });

    glfwSwapBuffers(window);
    glfwPollEvents();
    i += 1.0f;
  }

  triangle.destroy();
  gizmo.destroy();

  glfwTerminate();
}

/*
 * Use program
 * Bind VAO
 * Draw Array*/
// glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices2), vertices2);
