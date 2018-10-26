#include <glad/glad.h>
#include "helpers/camera.hpp"
#include <GLFW/glfw3.h>
#include <array>
#include <cmath>
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

  glfwPollEvents();
  glfwSetCursorPos(window, WIDTH / 2.0, HEIGHT / 2.0);
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
  bool loadout = loader.LoadFile("objects/sphere.obj");
  if (!loadout) {
    cout << "Failed to load file" << endl;
    return 1;
  }

  const auto &vertices = loader.LoadedVertices;
  const auto &indices = loader.LoadedIndices;

  // Color values: red light.
  GLfloat Light0Amb[4] = {0.6f, 0.2f, 0.1f, 1.0f};
  GLfloat Light0Dif[4] = {1.0f, 0.0f, 0.2f, 1.0f};
  GLfloat Light0Spec[4] = {0.4f, 0.4f, 0.4f, 1.0f};
  // Position values: puntual light.
  GLfloat Light0Pos[4] = {0.0f, 20.0f, 20.0f, 1.0f};
  GLfloat direction[] = {0.0, -1.0, -1.0};

  // Light0 parameters.
  glLightfv(GL_LIGHT0, GL_AMBIENT, Light0Amb);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, Light0Dif);
  glLightfv(GL_LIGHT0, GL_SPECULAR, Light0Spec);
  glLightfv(GL_LIGHT0, GL_POSITION, Light0Pos);

  // Activate light.
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

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
    glMultMatrixf(&camera.getProjectionMatrix()[0][0]);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMultMatrixf(&camera.getViewMatrix()[0][0]);

    drawGizmo();

    GLfloat MatAmbient[4] = {0.2f, 0.2f, 0.5f, 1.0f};
    GLfloat MatDiffuse[4] = {0.2f, 0.4f, 0.0f, 1.0f};
    GLfloat MatSpecular[4] = {1.2f, 1.2f, 1.2f, 1.0f};
    GLfloat MatShininess[] = {19.0F};
    glMaterialfv(GL_FRONT, GL_AMBIENT, MatAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, MatDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, MatSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, MatShininess);

    glInterleavedArrays(GL_T2F_N3F_V3F, sizeof(objl::Vertex), vertices.data());
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT,
                   indices.data());


    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
}