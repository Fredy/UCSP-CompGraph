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
using namespace std;

const GLuint WIDTH = 800, HEIGHT = 600;

const GLfloat floorAmbient[4] = {0.4f, 0.4f, 0.4f, 1.0f};
const GLfloat floorDiffuse[4] = {0.5f, 0.5f, 0.5f, 1.0f};
const GLfloat floorSpecular[4] = {0.2f, 0.2f, 0.2f, 1.0f};
const GLfloat floorShininess = 3.0f;

const GLfloat wallAmbient[4] = {0.4f, 0.4f, 0.4f, 1.0f};
const GLfloat wallDiffuse[4] = {0.2f, 0.2f, 0.2f, 1.0f};
const GLfloat wallSpecular[4] = {0.6f, 0.6f, 0.6f, 1.0f};
const GLfloat wallShininess = 13.0f;

const GLfloat roofAmbient[4] = {0.4f, 0.4f, 0.4f, 1.0f};
const GLfloat roofDiffuse[4] = {0.6f, 0.6f, 0.6f, 1.0f};
const GLfloat roofSpecular[4] = {0.8f, 0.8f, 0.8f, 1.0f};
const GLfloat roofShininess = 100.0f;

const GLfloat leavesAmbient[4] = {0.4f, 0.4f, 0.4f, 1.0f};
const GLfloat leavesDiffuse[4] = {0.3f, 0.3f, 0.3f, 1.0f};
const GLfloat leavesSpecular[4] = {0.9f, 0.9f, 0.9f, 1.0f};
const GLfloat leavesShininess = 8.0f;

const GLfloat logAmbient[4] = {0.4f, 0.4f, 0.4f};
const GLfloat logDiffuse[4] = {0.3f, 0.3f, 0.3f, 1.0f};
const GLfloat logSpecular[4] = {0.2f, 0.2f, 0.2f, 1.0f};
const GLfloat logShininess = 0.0f;

const GLfloat Light0Pos[] = {0.0f, 30.0f, 50.0f, 0.0f};

void frameBufferSizeCallback(GLFWwindow *window, int width, int height) {
  cout << "Width and height: " << width << ", " << height << "\n";
  glViewport(0, 0, width, height);
}

void setupLights() {
  // Color values: red light.
  GLfloat Light0Amb[] = {0.75f, 0.75f, 0.75f, 1.0f};
  GLfloat Light0Dif[] = {0.9f, 0.9f, 0.9f, 1.0f};
  GLfloat Light0Spec[] = {0.4f, 0.4f, 0.4f, 1.0f};
  // Position values: puntual light.

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
  // glEnable(GL_TEXTURE_2D);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);



  setupLights();
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

void drawFloor(GLuint textureId, float width, float height) {
  const float left = -width / 2.0f;
  const float bottom = -height / 2.0f;
  const float texProportion = 0.15f;

  glMaterialfv(GL_FRONT, GL_AMBIENT, floorAmbient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, floorDiffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, floorSpecular);
  glMaterialf(GL_FRONT, GL_SHININESS, floorShininess);

  glBindTexture(GL_TEXTURE_2D, textureId);
  glBegin(GL_QUADS);
  glNormal3f(0,1,0);
  glTexCoord2f(left * texProportion, bottom *texProportion);
  glVertex3f(left, 0, bottom);

  glTexCoord2f(left * texProportion, (bottom + height) * texProportion);
  glVertex3f(left, 0, bottom + height);

  glTexCoord2f((left + width) * texProportion, (bottom + height) * texProportion);
  glVertex3f(left + width, 0, bottom + height);

  glTexCoord2f((left + width) * texProportion, bottom * texProportion);
  glVertex3f(left + width, 0, bottom);
  glEnd();
}

// Width: X
// Height: Y
// Length: Z
void drawWalls(GLuint textureId, float x, float y, float z, float width, float height,
               float length) {

  glMaterialfv(GL_FRONT, GL_AMBIENT, wallAmbient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, wallDiffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, wallSpecular);
  glMaterialf(GL_FRONT, GL_SHININESS, wallShininess);

  glBindTexture(GL_TEXTURE_2D, textureId);
  glBegin(GL_QUADS);

  glTexCoord2f(0, 0);
  glNormal3f(0,0,-1);
  glVertex3f(x, y, z);
  glTexCoord2f(0, 8);
  glVertex3f(x, y + height, z);
  glTexCoord2f(8, 8);
  glVertex3f(x + width, y + height, z);
  glTexCoord2f(8, 0);
  glVertex3f(x + width, y, z);


  glNormal3f(0,0,1);
  glTexCoord2f(0, 0);
  glVertex3f(x, y, z + length);
  glTexCoord2f(8, 0);
  glVertex3f(x + width, y, z + length);
  glTexCoord2f(8, 8);
  glVertex3f(x + width, y + height, z + length);
  glTexCoord2f(0, 8);
  glVertex3f(x, y + height, z + length);

  glNormal3f(-1,0,0);
  glTexCoord2f(0, 0);
  glVertex3f(x, y, z);
  glTexCoord2f(8, 0);
  glVertex3f(x, y, z + length);
  glTexCoord2f(8, 8);
  glVertex3f(x , y + height, z + length);
  glTexCoord2f(0, 8);
  glVertex3f(x, y + height, z);

  glNormal3f(1,0,0);
  glTexCoord2f(0, 0);
  glVertex3f(x + width, y, z);
  glTexCoord2f(0, 8);
  glVertex3f(x + width, y + height, z );
  glTexCoord2f(8, 8);
  glVertex3f(x + width, y + height, z + length);
  glTexCoord2f(8, 0);
  glVertex3f(x + width, y, z + length);

  glEnd();
}

void drawRoof(GLuint wallTextureId,GLuint roofTextureId, float x, float y, float z, float width, float height, float length) {
  glMaterialfv(GL_FRONT, GL_AMBIENT, wallAmbient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, wallDiffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, wallSpecular);
  glMaterialf(GL_FRONT, GL_SHININESS, wallShininess);

  glBindTexture(GL_TEXTURE_2D, wallTextureId) ;
  glBegin(GL_TRIANGLES);
  glNormal3f(0, 0, -1);
  glTexCoord2f(0, 0);
  glVertex3f(x, y, z);
  glTexCoord2f(3, 6);
  glVertex3f(x + width / 2.0f, y + height, z);
  glTexCoord2f(6, 0);
  glVertex3f(x + width, y, z);

  glNormal3f(0, 0, 1);
  glTexCoord2f(0, 0);
  glVertex3f(x, y, z + length);
  glTexCoord2f(6, 0);
  glVertex3f(x + width, y, z + length);
  glTexCoord2f(3, 6);
  glVertex3f(x + width/ 2.0f, y + height, z + length);

  glMaterialfv(GL_FRONT, GL_AMBIENT, roofAmbient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, roofDiffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, roofSpecular);
  glMaterialf(GL_FRONT, GL_SHININESS, roofShininess);

  glEnd();

  glBindTexture(GL_TEXTURE_2D, roofTextureId) ;
  glBegin(GL_QUADS);

  glm::vec3 one(0, 0, -length);
  glm::vec3 two(width / 2.0f, height, 0);
  glNormal3fv(&glm::normalize(glm::cross(two, one))[0]);
  glTexCoord2f(0, 0);
  glVertex3f(x, y, z); // 0
  glTexCoord2f(0, 8);
  glVertex3f(x, y, z + length); // 1
  glTexCoord2f(8, 8);
  glVertex3f(x + width /2.0f, y + height, z + length); // 2
  glTexCoord2f(8, 0);
  glVertex3f(x + width /2.0f, y + height, z);

  one = glm::vec3(width / 2.0f, -height, 0);
  two = glm::vec3(0, 0, length);
  glNormal3fv(&glm::normalize(glm::cross(two, one))[0]);
  glTexCoord2f(0, 0);
  glVertex3f(x + width, y, z); // 0
  glTexCoord2f(8, 0);
  glVertex3f(x + width /2.0f, y + height, z); // 1
  glTexCoord2f(8, 8);
  glVertex3f(x + width /2.0f, y + height, z + length); // 2
  glTexCoord2f(0, 8); 
  glVertex3f(x + width, y, z + length);

  glEnd();
}

void drawHouse(GLuint wallTextureId, GLuint roofTextureId,  float x, float y, float z, float width, float wallHeight, float roofHeight, float length) {
  drawWalls(wallTextureId, x,y,z,width, wallHeight, length);
  drawRoof(wallTextureId, roofTextureId, x, y + wallHeight,z , width, roofHeight, length);
}

template <typename V, typename I>
void drawTree(GLuint leavesTextureId, GLuint barkTextureId, const V &cylinderVertices, const I &cylinderIndices,
              const V &sphereVertices, const I &sphereIndices) {
  glPushMatrix();
  glTranslatef(10.0f, 0.0f, 10.0f);

  glMaterialfv(GL_FRONT, GL_AMBIENT, leavesAmbient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, leavesDiffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, leavesSpecular);
  glMaterialf(GL_FRONT, GL_SHININESS, leavesShininess);

  glPushMatrix();
  glTranslatef(0.0f, 8.0f, 0.0f);
  glScalef(0.2f, 0.2f, 0.2f);

  glBindTexture(GL_TEXTURE_2D, leavesTextureId);
  glInterleavedArrays(GL_T2F_N3F_V3F, sizeof(objl::Vertex),
                      sphereVertices.data());
  glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT,
                 sphereIndices.data());

  glPopMatrix();

  glMaterialfv(GL_FRONT, GL_AMBIENT, logAmbient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, logDiffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, logSpecular);
  glMaterialf(GL_FRONT, GL_SHININESS, logShininess);
  glPushMatrix();
  // glTranslatef(0.0f, 2.0f,0.0f);
  glScalef(1.0f, 3.0f, 1.0f);
  glBindTexture(GL_TEXTURE_2D, barkTextureId);
  glInterleavedArrays(GL_T2F_N3F_V3F, sizeof(objl::Vertex),
                      cylinderVertices.data());
  glDrawElements(GL_TRIANGLES, cylinderIndices.size(), GL_UNSIGNED_INT,
                 cylinderIndices.data());
  glPopMatrix();
  glPopMatrix();
}

int main() {
  GLFWwindow *window = initGL();
  if (!window) {
    return 1;
  }

  // Load models.
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

  // Load textures.
  GLuint barkTex;
  GLuint brickTex;
  GLuint grassTex;
  GLuint leavesTex;
  GLuint roofTex;

  try {
    barkTex = texture::load("textures/bark.jpg");
    brickTex = texture::load("textures/brick.jpg");
    grassTex = texture::load("textures/grass.jpg");
    leavesTex = texture::load("textures/leaves.jpg");
    roofTex = texture::load("textures/roof.jpg");
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    return 1;
  }

  Camera camera(window, 10.0f);
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

    drawFloor(grassTex, 40, 40);
    drawHouse(brickTex, roofTex, -10, 0, -7.5f, 10, 10, 5, 15);
    drawTree(leavesTex, barkTex, cylinderVertices, cylinderIndices, sphereVertices, sphereIndices);

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