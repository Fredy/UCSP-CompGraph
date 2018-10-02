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

GLuint WIDTH = 800, HEIGHT = 600;

int cameraX = 0, cameraZ = 0;
float cameraAtX = 0, cameraAtY = 0;

void frameBufferSizeCallback(GLFWwindow *window, int width, int height) {
  cout << "Width and height: " << width << ", " << height << "\n";
  WIDTH = width;
  HEIGHT = height;
  glViewport(0, 0, width, height);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_UP && action == GLFW_REPEAT) {
    cameraZ--;
  }
  if (key == GLFW_KEY_DOWN && action == GLFW_REPEAT) {
    cameraZ++;
  }
  if (key == GLFW_KEY_LEFT && action == GLFW_REPEAT) {
    cameraX--;
  }
  if (key == GLFW_KEY_RIGHT && action == GLFW_REPEAT) {
    cameraX++;
  }
}

static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
  double centerX = WIDTH / 2.0;
  double centerY = HEIGHT / 2.0;
  //glfwSetCursorPos(window, centerX, centerY);
  float speed = 0.01f;
  cameraAtX = speed * float(centerX - xpos);
  cameraAtY = speed * float(centerY - ypos);

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

  glfwSetKeyCallback(window, keyCallback);
  glfwSetCursorPosCallback(window, cursorPosCallback);
  // Glad initialization
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    cout << "Failed to initialize GLAD\n";
    return 1;
  }

  //////////////////////////////////////////////////////////////////////

  // Build and compile shader programs.
  Shader shaders("shaders/vshader.glsl", "shaders/fshader.glsl");


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


  Object gizmo;
  gizmo.setVerticesData(6, verticesGizmo, colorsGizmo);
  gizmo.setMode(GL_LINES);
  gizmo.setShader(shaders.ID);


  // MVP
  glm::mat4 projectionMat = glm::perspective(
      glm::radians(70.0f), float(WIDTH) / HEIGHT, 0.1f, 250.0f);
  // For ortho camera:
  // glm::mat4 projectionMat = glm::ortho(-400.0f, 400.0f, -300.0f, 300.0f, 0.0f,
                                      //  100.0f); // In world coordinates

  glm::mat4 viewMat = glm::mat4(1.0);// glm::lookAt(glm::vec3{90, 90, 90}, {cameraX, 0, cameraZ}, {0, 1, 0});

  glm::mat4 modelMat = glm::mat4(1.0f);
  //modelMat = glm::scale(modelMat, {0.2f, 0.2f, 0.2f});

  glm::mat4 mvp = projectionMat * viewMat * modelMat;
  GLuint matrixId = glGetUniformLocation(shaders.ID, "mvp");

  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  // Main loop
  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);

  // viewMat = glm::lookAt(glm::vec3{0, 90, 90}, glm::vec3{cameraX, 0, cameraZ}, {0, 1, 0});

    //  XYZ
    gizmo.draw([&] {
      modelMat = glm::mat4(1.0f);
      modelMat = glm::translate(modelMat, {cameraX, 0, cameraZ - 90});
      modelMat = glm::rotate(modelMat, cameraAtX, {0, 1, 0});
      modelMat = glm::rotate(modelMat, cameraAtY, {1, 0, 0});
      mvp = projectionMat * viewMat * modelMat;
      glUniformMatrix4fv(matrixId, 1, GL_FALSE, &mvp[0][0]);
    });

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  gizmo.destroy();

  glfwTerminate();
}

/*
 * Use program
 * Bind VAO
 * Draw Array*/
// glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices2), vertices2);
