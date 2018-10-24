#include <glad/glad.h>
#include "helpers/camera.hpp"
#include <array>
#include <cmath>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <objLoader/OBJ_Loader.h>
#include <vector>
using namespace std;

const GLuint WIDTH = 800, HEIGHT = 600;

void frameBufferSizeCallback(GLFWwindow *window, int width, int height) {
  cout << "Width and height: " << width << ", " << height << "\n";
  glViewport(0, 0, width, height);
}

GLFWwindow *initGL() {
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
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glEnable(GL_DEPTH_TEST);
  // Accept fragment if it closer to the camera than the former one
  glDepthFunc(GL_LESS);
  glEnable(GL_CULL_FACE);

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

void drawFloor(float width, float height) {
  const float left = -width / 2.0f;
  const float bottom = -height / 2.0f;
  glBegin(GL_QUADS);
  glColor3f(0.2f, 0.8f, 0.1f);
  glVertex3f(left, 0, bottom);
  glVertex3f(left, 0, bottom + height);
  glVertex3f(left + width, 0, bottom + height);
  glVertex3f(left + width, 0, bottom);
  glEnd();
}

// Width: X
// Height: Y
// Length: Z
void drawWalls(float x, float y, float z, float width, float height,
               float length) {
  glBegin(GL_QUADS);
  glColor3f(0.8f, 0.1f, 0.2f);

  glVertex3f(x, y, z);
  glVertex3f(x, y + height, z);
  glVertex3f(x + width, y + height, z);
  glVertex3f(x + width, y, z);

  glVertex3f(x, y, z + length);
  glVertex3f(x + width, y, z + length);
  glVertex3f(x + width, y + height, z + length);
  glVertex3f(x, y + height, z + length);

  glVertex3f(x, y, z);
  glVertex3f(x, y, z + length);
  glVertex3f(x , y + height, z + length);
  glVertex3f(x, y + height, z);

  glVertex3f(x + width, y, z);
  glVertex3f(x + width, y + height, z );
  glVertex3f(x + width, y + height, z + length);
  glVertex3f(x + width, y, z + length);

  glEnd();
}

void drawRoof(float x, float y, float z, float width, float height, float length) {
  glBegin(GL_TRIANGLES);
  glColor3f(0.8f, 0.1f, 0.2f);

  glVertex3f(x, y, z);
  glVertex3f(x + width / 2.0f, y + height, z);
  glVertex3f(x + width, y, z);

  glVertex3f(x, y, z + length);
  glVertex3f(x + width, y, z + length);
  glVertex3f(x + width/ 2.0f, y + height, z + length);

  glEnd();

  glBegin(GL_QUADS);
  glColor3f(0.5f, 0.5f, 0.5f);

  glVertex3f(x, y, z);
  glVertex3f(x, y, z + length);
  glVertex3f(x + width /2.0f, y + height, z + length);
  glVertex3f(x + width /2.0f, y + height, z);

  glVertex3f(x + width, y, z);
  glVertex3f(x + width /2.0f, y + height, z);
  glVertex3f(x + width /2.0f, y + height, z + length);
  glVertex3f(x + width, y, z + length);

  glEnd();
}

void drawHouse(float x, float y, float z, float width, float wallHeight, float roofHeight, float length) {
  drawWalls(x,y,z,width, wallHeight, length);
  drawRoof(x, y + wallHeight,z , width, roofHeight, length);
}

int main() {
  GLFWwindow *window = initGL();
  if (!window) {
    return 1;
  }

  objl::Loader loader;
  bool loadout = loader.LoadFile("objects/sphere.obj");
  if (!loadout) {
    cout << "Failed to load file" << endl;
    return 1;
  }

  auto sphereVertices = loader.LoadedVertices;
  auto sphereIndices = loader.LoadedIndices;

  loadout = loader.LoadFile("objects/cylinder.obj");
  if (!loadout) {
    cout << "Failed to load file" << endl;
    return 1;
  }

  auto cylinderVertices = loader.LoadedVertices;
  auto cylinderIndices = loader.LoadedIndices;

  // MVP
  glm::mat4 projectionMat = glm::perspective(
      glm::radians(90.0f), float(WIDTH) / HEIGHT, 0.1f, 250.0f);
  glm::mat4 viewMat = glm::lookAt(glm::vec3{10, 10, 10}, {0, 0, 0}, {0, 1, 0});
  glm::mat4 modelMat = glm::mat4(1.0f);

  Camera camera(window, 10.0f);
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
    // glMultMatrixf(&projectionMat[0][0]);
    glMultMatrixf(&camera.getProjectionMatrix()[0][0]);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // glMultMatrixf(&viewMat[0][0]);
    glMultMatrixf(&camera.getViewMatrix()[0][0]);

    drawFloor(40, 40);
    drawHouse(-10, 0, -7.5f, 10, 10, 5, 15);

    glTranslatef(10.0f, 0.0f, 10.0f);

    glColor3f(0.2f, 0.9f, 0.1f);
    glPushMatrix();
    glTranslatef(0.0f, 8.0f,0.0f);
    glScalef(0.2f, 0.2f, 0.2f);
    glInterleavedArrays(GL_T2F_N3F_V3F, sizeof(objl::Vertex),
                        sphereVertices.data());
    glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT,
                   sphereIndices.data());

    glPopMatrix();

    glColor3f(0.35f, 0.15f, 0.005f);
    glPushMatrix();
    // glTranslatef(0.0f, 2.0f,0.0f);
    glScalef(1.0f, 3.0f, 1.0f);
    glInterleavedArrays(GL_T2F_N3F_V3F, sizeof(objl::Vertex),
                        cylinderVertices.data());
    glDrawElements(GL_TRIANGLES, cylinderIndices.size(), GL_UNSIGNED_INT,
                   cylinderIndices.data());
    glPopMatrix();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
}