#include "helpers/object.hpp"
#include "helpers/shader.hpp"
#include "helpers/objloader.hpp"
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

  std::vector< glm::vec3 > vertices;
  std::vector<glm::vec2> uvs;
  std::vector<glm::vec3> normals;
  bool complete = loadOBJ("objects/sphere.obj", vertices, uvs, normals);

  ///////////////////
  // Icosahedron Test
  GLuint vao, vbo;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  glBindVertexArray(vao);
  // Points
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glEnableVertexAttribArray(0);

  Shader oneColorShader("shaders/lab2_one_color_v.glsl", "shaders/lab2_one_color_f.glsl");
  GLint colorId = glGetUniformLocation(oneColorShader.ID, "inColor");

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  Object sphere;
  sphere.setExternalData(vao, vbo, 0, vertices.size());
  sphere.setShader(oneColorShader.ID);
  sphere.setMode(GL_TRIANGLES);
  ////////////
  ////////////

  //Shader simpleShader("shaders/minimal_v.glsl", "shaders/minimal_f.glsl");
  Object gizmo;
  gizmo.setVerticesData(6, verticesGizmo, colorsGizmo);
  gizmo.setMode(GL_LINES);
  gizmo.setShader(shaders.ID);


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
  GLuint matrixIdOneColorShader = glGetUniformLocation(oneColorShader.ID, "mvp");

  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
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

    // Sun
    sphere.draw([&] {
      modelMat = glm::mat4(1.0f);
      modelMat = glm::rotate(modelMat, glm::radians(i), {0.0f, 1.0f, 0.0f});
      mvp = projectionMat * viewMat * modelMat;
      glUniform3f(colorId, 1.0f, 0.91f, 0.05f);
      glUniformMatrix4fv(matrixIdOneColorShader, 1, GL_FALSE, &mvp[0][0]);
    });

    auto earthRT = [&] {
      modelMat = glm::rotate(modelMat, glm::radians(i), {0.0f, 1.0f, 0.0f});
      modelMat = glm::translate(modelMat, {80.0f, 0.0f, 0.0f});
    };
    // Earth
    sphere.draw([&] {
      modelMat = glm::mat4(1.0f);
      earthRT();
      // Rotation in it self.
      modelMat = glm::rotate(modelMat, glm::radians(3.0f *i), {0.0f, 1.0f, 0.0f});
      modelMat = glm::scale(modelMat, {0.5f, 0.5f, 0.5f});
      mvp = projectionMat * viewMat * modelMat;
      glUniform3f(colorId, 0.16f, 0.23f, 0.88f);
      glUniformMatrix4fv(matrixIdOneColorShader, 1, GL_FALSE, &mvp[0][0]);
    });

    // Moon
    sphere.draw([&] {
      modelMat = glm::mat4(1.0f);
      earthRT();
      modelMat = glm::rotate(modelMat, glm::radians(2.0f * i), {0.5f, 1.0f, 0.0f});
      modelMat = glm::translate(modelMat, {0.0f, 0.0f, 30.0f});
      // Rotation in it self.
      modelMat = glm::rotate(modelMat, glm::radians(1.5f * i), {0.0f, 1.0f, 0.0f});
      modelMat = glm::scale(modelMat, {0.2f, 0.2f, 0.2f});
      mvp = projectionMat * viewMat * modelMat;
      glUniform3f(colorId, 0.1f, 0.07f, 0.1f);
      glUniformMatrix4fv(matrixIdOneColorShader, 1, GL_FALSE, &mvp[0][0]);
    });

    // Mars
    sphere.draw([&] {
      modelMat = glm::mat4(1.0f);
      modelMat = glm::rotate(modelMat, glm::radians(i), {0.0f, 1.0f, 0.0f});
      modelMat = glm::translate(modelMat, {120.0f, 0.0f, 0.0f});
      // Rotation in it self.
      modelMat = glm::rotate(modelMat, glm::radians(i), {0.0f, 1.0f, 0.3f});
      modelMat = glm::scale(modelMat, {0.5f, 0.5f, 0.5f});
      mvp = projectionMat * viewMat * modelMat;
      glUniform3f(colorId, 1.0f, 0.13f, 0.1f);
      glUniformMatrix4fv(matrixIdOneColorShader, 1, GL_FALSE, &mvp[0][0]);
    });

    glfwSwapBuffers(window);
    glfwPollEvents();
    i += 1.0f;
  }

  gizmo.destroy();

  glfwTerminate();
}

/*
 * Use program
 * Bind VAO
 * Draw Array*/
// glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices2), vertices2);
