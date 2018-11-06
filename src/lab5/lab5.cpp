#include <glad/glad.h>
#include "helpers/camera.hpp"
#include <array>
#include <cmath>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <objLoader/OBJ_Loader.h>
#include "helpers/texture.hpp"
#include <vector>
#include "fileReader.hpp"
using namespace std;

const GLuint WIDTH = 800, HEIGHT = 600;

const GLfloat floorAmbient[4] = {0.5f, 0.5f, 0.5f, 1.0f};
const GLfloat floorDiffuse[4] = {0.7f, 0.7f, 0.7f, 1.0f};
const GLfloat floorSpecular[4] = {0.9f, 0.9f, 0.9f, 1.0f};
const GLfloat floorShininess = 3.0f;

GLfloat Light0Pos[] = {0.0f, 1000.0f, 5000.0f, 0.0f};

struct Vertex {
  glm::vec2 texCoord;
  glm::vec3 normal;
  glm::vec3 coordinates;

  Vertex(const glm::vec2 &texCoord, const glm::vec3 &normal,
         const glm::vec3 &coordinates) {
    this->texCoord = texCoord;
    this->normal = normal;
    this->coordinates = coordinates;
  }
};

void frameBufferSizeCallback(GLFWwindow *window, int width, int height) {
  cout << "Width and height: " << width << ", " << height << "\n";
  glViewport(0, 0, width, height);
}

void setupLights() {
  // Color values: red light.
  GLfloat Light0Amb[] = {0.75f, 0.75f, 0.75f, 1.0f};
  GLfloat Light0Dif[] = {0.9f, 0.9f, 0.9f, 1.0f};
  GLfloat Light0Spec[] = {0.4f, 0.4f, 0.4f, 1.0f};

  // Light0 parameters.
  glLightfv(GL_LIGHT0, GL_AMBIENT, Light0Amb);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, Light0Dif);
  glLightfv(GL_LIGHT0, GL_SPECULAR, Light0Spec);
  glLightfv(GL_LIGHT0, GL_POSITION, Light0Pos);

  // Activate light.
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
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
  glEnable(GL_TEXTURE_2D);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);

  setupLights();
  return window;
}

void drawGizmo() {
  glBindTexture(GL_TEXTURE_2D, 0);
  glDisable(GL_LIGHTING);
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
  glEnable(GL_LIGHTING);
}

int main() {
  GLFWwindow *window = initGL();
  if (!window) {
    return 1;
  }

  auto fileData = mesh::read("objects/fontvieille.txt");
  auto indices = mesh::genIndices(fileData.yVals.size(), fileData.yVals.at(0).size());
  auto points = mesh::genMesh(fileData.yVals, fileData.xStep, fileData.zStep);
  auto normals = mesh::genNormals(indices, points);
  auto texCoords = mesh::genTexCoords(fileData.yVals.size(), fileData.yVals.at(0).size());

  vector<Vertex> vertices;
  vertices.reserve(points.size());
  for (size_t i = 0 ; i < points.size(); i++) {
    vertices.emplace_back(texCoords[i], normals[i], points[i]);
  }

  GLuint mapTextureId =  texture::load("textures/fontvieille_1.tga");

  Camera camera(window, 500.0f);



  // Main loop
  double dt, currentTime, lastTime = 0.0, vel = 20.0, tmp = 0.0;
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

    drawGizmo();

    glBindTexture(GL_TEXTURE_2D, mapTextureId);
    glMaterialfv(GL_FRONT, GL_AMBIENT, floorAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, floorDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, floorSpecular);
    glMaterialf(GL_FRONT, GL_SHININESS, floorShininess);

    glInterleavedArrays(GL_T2F_N3F_V3F, sizeof(Vertex), vertices.data());
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data());

    glPushMatrix();
    tmp += vel * dt;
    glRotatef(tmp, 0.0f, 1.0f, 0.0f);
    glLightfv(GL_LIGHT0, GL_POSITION, Light0Pos);
    glPopMatrix();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
}