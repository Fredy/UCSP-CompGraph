#include <glad/glad.h>
#include "helpers/camera.hpp"
#include <objLoader/OBJ_Loader.h>
#include <GLFW/glfw3.h>
#include <array>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
using namespace std;

const GLuint WIDTH = 800, HEIGHT = 600;

void frameBufferSizeCallback(GLFWwindow *window, int width, int height) {
  cout << "Width and height: " << width << ", " << height << "\n";
  glViewport(0, 0, width, height);
}

GLFWwindow * initGL() {
 // GLFW initialization
  if (!glfwInit()) {
    cout << "GLFW initialization failed\n";
    glfwTerminate();
    return nullptr;
  }

  // GLFW setup
  glfwWindowHint(GLFW_SAMPLES, 4); // Anti aliasing
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

  GLFWwindow *window =
      glfwCreateWindow(WIDTH, HEIGHT, "Test window", nullptr, nullptr);
  if (!window) {
    cout << "GLFW windows creation failed failed\n";
    glfwTerminate();
    return nullptr;
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  // Glad initialization
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    cout << "Failed to initialize GLAD\n";
    glfwTerminate();
    return nullptr;
  }

  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glEnable(GL_DEPTH_TEST);
  // Accept fragment if it closer to the camera than the former one
  glDepthFunc(GL_LESS);

  return window;
}

void drawGizmo() {
  glBegin(GL_LINES);
  glColor3d(1, 0, 0);
  glVertex3d(0, 0, 0);
  glVertex3d(255, 0, 0);
  glColor3d(0, 1, 0);
  glVertex3d(0, 0, 0);
  glVertex3d(0, 255, 0);
  glColor3d(0, 0, 1);
  glVertex3d(0, 0, 0);
  glVertex3d(0, 0, 255);
  glEnd();
}

int main() {
  GLFWwindow *window = initGL();
  if (!window) {
    return 1;
  }

  objl::Loader loader;
  bool loadout = loader.LoadFile("objects/cube.obj");
  if (!loadout) {
    cout << "Failed to load file" << endl;
    return 1;
  }

  auto vertices = loader.LoadedVertices;
  auto indices = loader.LoadedIndices;

  // MVP
  glm::mat4 projectionMat = glm::perspective(
      glm::radians(90.0f), float(WIDTH) / HEIGHT, 0.1f, 250.0f);
  glm::mat4 viewMat = glm::lookAt(glm::vec3{10, 10, 10}, {0, 0, 0}, {0, 1, 0});
  glm::mat4 modelMat = glm::mat4(1.0f);

  Camera camera(window);
  // Main loop
  double dt, currentTime, lastTime = 0.0;
  while (!glfwWindowShouldClose(window)) {
    currentTime = glfwGetTime();
    dt = currentTime - lastTime;
    lastTime = currentTime;


    camera.computeMatrices(dt);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //glMultMatrixf(&projectionMat[0][0]);
    glMultMatrixf(&camera.getProjectionMatrix()[0][0]);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //glMultMatrixf(&viewMat[0][0]);
    glMultMatrixf(&camera.getViewMatrix()[0][0]);

    drawGizmo();

    glInterleavedArrays(GL_T2F_N3F_V3F, sizeof(objl::Vertex),vertices.data());
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data());

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
}