#include "helpers/object.hpp"
#include "helpers/objloader.hpp"
#include "helpers/shader.hpp"
#include "objLoader/OBJ_Loader.h"
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

Object genGizmo(unsigned int shaderId) {
  // Vertex data
  GLfloat verticesGizmo[] = {
      0.0f,  0.0f,  0.0f, // X
      80.0f, 0.0f,  0.0f, //
      0.0f,  0.0f,  0.0f, // Y
      0.0f,  80.0f, 0.0f, //
      0.0f,  0.0f,  0.0f, // Z
      0.0f,  0.0f,  80.0f,
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
  gizmo.setShader(shaderId);

  return gizmo;
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

  // Build and compile shader programs.
  Shader shaders("shaders/vshader.glsl", "shaders/fshader.glsl");
  GLuint matrixId = glGetUniformLocation(shaders.ID, "mvp");

  Shader oneColorShader("shaders/objLoad_v.glsl",
                        "shaders/objLoad_f.glsl");
  GLint colorId = glGetUniformLocation(oneColorShader.ID, "inColor");
  GLuint matrixIdOneColorShader =
      glGetUniformLocation(oneColorShader.ID, "mvp");

  //////////////////////////////////////////////////////////////////////
  objl::Loader loader;
  bool loadout = loader.LoadFile("objects/cube.obj");
  if (!loadout) {
    cout << "Failed to load file" << endl;
    return 1;
  }

  auto vertices = loader.LoadedVertices;
  auto indices = loader.LoadedIndices;
  unsigned int VAO, VBO, EBO;
  // create buffers/arrays
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);
  // load data into vertex buffers
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  // A great thing about structs is that their memory layout is sequential for
  // all its items. The effect is that we can simply pass a pointer to the
  // struct and it translates perfectly to a glm::vec3/2 array which again
  // translates to 3/2 floats which translates to a byte array.
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(objl::Vertex),
               &vertices[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               &indices[0], GL_STATIC_DRAW);

  // set the vertex attribute pointers
  // vertex Positions
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex),
                        (void *)0);
  // vertex normals
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex),
                        (void *)offsetof(objl::Vertex, Normal));
  // vertex texture coords
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex),
                        (void *)offsetof(objl::Vertex, TextureCoordinate));

  glBindVertexArray(0);
  //////////////////////////////////////////////////////////////////////

  Object gizmo = genGizmo(shaders.ID);

  // MVP
  glm::mat4 projectionMat = glm::perspective(
      glm::radians(90.0f), float(WIDTH) / HEIGHT, 0.1f, 250.0f);
  glm::mat4 viewMat = glm::lookAt(glm::vec3{10, 10, 10}, {0, 0, 0}, {0, 1, 0});

  glm::mat4 modelMat = glm::mat4(1.0f);

  glm::mat4 mvp = projectionMat * viewMat * modelMat;

  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glEnable(GL_DEPTH_TEST);
  // Accept fragment if it closer to the camera than the former one
  glDepthFunc(GL_LESS);
  float i = 0.0f;
  // Main loop
  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //  XYZ
    gizmo.draw([&] {
      modelMat = glm::mat4(1.0f);
      mvp = projectionMat * viewMat * modelMat;
      glUniformMatrix4fv(matrixId, 1, GL_FALSE, &mvp[0][0]);
    });

    ///////////////////////////

    // draw mesh
    glUseProgram(oneColorShader.ID);
    glBindVertexArray(VAO);

    modelMat = glm::mat4(1.0f);
    mvp = projectionMat * viewMat * modelMat;
    glUniformMatrix4fv(matrixIdOneColorShader, 1, GL_FALSE, &mvp[0][0]);
    glUniform3f(colorId, 0.0f, 0.0f, 0.88f);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glUseProgram(0);


    ///////////////////////////

    glfwSwapBuffers(window);
    glfwPollEvents();
    i += 1.0f;
  }

  gizmo.destroy();

  glfwTerminate();
}