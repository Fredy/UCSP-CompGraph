#include "helpers/object.hpp"
#include "helpers/shader.hpp"
#include <GLFW/glfw3.h>
#include <array>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
using namespace std;

GLint WIDTH = 800, HEIGHT = 600;

void frameBufferSizeCallback(GLFWwindow *window, int width, int height) {
  cout << "Width and height: " << width << ", " << height << "\n";
  WIDTH = width;
  HEIGHT = height;
  glViewport(0, 0, width, height);
}

array<float, 3> processInput(GLFWwindow *window, float arm1Angle,
                             float arm2Angle, float ropeScale) {
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
    arm2Angle += 1.0f;
  }
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
    arm2Angle -= 1.0f;
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
    arm1Angle += 1.0f;
  }
  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
    arm1Angle -= 1.0f;
  }
  if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS) {
    ropeScale += 0.01f;
  }
  if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) {
    ropeScale -= 0.01f;
  }

  return {arm1Angle, arm2Angle, ropeScale};
}

Object buildRectangle(float left, float bottom, float width, float height,
                      GLuint shaderId) {
  GLfloat vertices[] = {
      left,         bottom,          0.0f, // bl
      left + width, bottom,          0.0f, // br
      left + width, bottom + height, 0.0f, // tr

      left,         bottom,          0.0f, // bl
      left + width, bottom + height, 0.0f, // tr
      left,         bottom + height, 0.0f, // tl
  };
  Object rectangle;
  rectangle.setVerticesData(6, vertices);
  rectangle.setMode(GL_TRIANGLES);
  rectangle.setShader(shaderId);

  return rectangle;
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

  float blue[3] = {0.16f, 0.23f, 0.88f};
  float orange[3] = {1.0f, 0.67f, 0.0f};
  float red[3] = {0.81f, 0.02f, 0.0f};
  float black[3] = {0.0f, 0.0f, 0.0f};

  Shader oneColorShader("shaders/lab2_one_color_v.glsl",
                        "shaders/lab2_one_color_f.glsl");

  float baseW = 70.0f;
  float baseH = 35.0f;
  float arm1W = 17.5f;
  float arm1H = 120.0f;
  float arm2W = 80.0f;
  float arm2H = arm1W;
  float boxH = 35.0f;
  float boxW = boxH;
  float ropeH = 80.0f;

  Object base = buildRectangle(0, 0, baseW, baseH, oneColorShader.ID);
  Object arm1 =
      buildRectangle(-arm1W / 2.0f, 0, arm1W, arm1H, oneColorShader.ID);
  Object arm2 =
      buildRectangle(0, -arm2H / 2.0f, arm2W, arm2H, oneColorShader.ID);
  Object box = buildRectangle(-boxW / 2.0f, 0, boxW, boxH, oneColorShader.ID);

  GLfloat ropeVertices[] = {
      0.0f, -ropeH, 0.0f, 0.0f, 0.0f, 0.0f,
  };

  Object rope;
  rope.setVerticesData(2, ropeVertices);
  rope.setMode(GL_LINES);
  rope.setShader(oneColorShader.ID);

  Object gizmo;
  gizmo.setVerticesData(6, verticesGizmo, colorsGizmo);
  gizmo.setMode(GL_LINES);
  gizmo.setShader(shaders.ID);

  // MVP
  glm::mat4 projectionMat = glm::ortho(-400.0f, 400.0f, -300.0f, 300.0f, 0.0f,
                                       100.0f); // In world coordinates

  glm::mat4 viewMat = glm::lookAt(glm::vec3{00, 00, 1}, {0, 0, 0}, {0, 1, 0});

  glm::mat4 modelMat = glm::mat4(1.0f);

  glm::mat4 mvp = projectionMat * viewMat * modelMat;
  GLuint matrixId = glGetUniformLocation(shaders.ID, "mvp");
  GLuint matrixIdOneColorShader =
      glGetUniformLocation(oneColorShader.ID, "mvp");
  GLint colorId = glGetUniformLocation(oneColorShader.ID, "inColor");

  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  // Main loop
  float arm1Angle = -30.0f;
  float arm2Angle = 40.0f;
  float ropeScale = 1.0f;
  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);
    projectionMat = glm::ortho(-WIDTH / 4.0f, WIDTH / 4.0f, -HEIGHT / 4.0f,
                               HEIGHT / 4.0f, 0.0f,
                               100.0f); // In world coordinates
    auto newVals = processInput(window, arm1Angle, arm2Angle, ropeScale);
    arm1Angle = newVals[0];
    arm2Angle = newVals[1];
    ropeScale = newVals[2];

    //  XYZ
    gizmo.draw([&] {
      modelMat = glm::mat4(1.0f);
      mvp = projectionMat * viewMat * modelMat;
      glUniformMatrix4fv(matrixId, 1, GL_FALSE, &mvp[0][0]);
    });

    modelMat = glm::mat4(1.0f);

    base.draw([&] {
      mvp = projectionMat * viewMat * modelMat;
      glUniform3fv(colorId, 1, blue);
      glUniformMatrix4fv(matrixIdOneColorShader, 1, GL_FALSE, &mvp[0][0]);
    });

    modelMat = glm::translate(modelMat, {baseW / 2.0f, baseH, 0.0f});
    modelMat =
        glm::rotate(modelMat, glm::radians(arm1Angle), {0.0f, 0.0f, 1.0f});

    arm1.draw([&] {
      mvp = projectionMat * viewMat * modelMat;
      glUniform3fv(colorId, 1, orange);
      glUniformMatrix4fv(matrixIdOneColorShader, 1, GL_FALSE, &mvp[0][0]);
    });

    modelMat = glm::translate(modelMat, {0.0f, arm1H, 0.0f});
    modelMat =
        glm::rotate(modelMat, glm::radians(arm2Angle), {0.0f, 0.0f, 1.0f});

    arm2.draw([&] {
      mvp = projectionMat * viewMat * modelMat;
      glUniform3fv(colorId, 1, orange);
      glUniformMatrix4fv(matrixIdOneColorShader, 1, GL_FALSE, &mvp[0][0]);
    });

    modelMat = glm::translate(modelMat, {arm2W, -arm2H / 2.0, 0.0f});
    // Undo rotations
    modelMat =
        glm::rotate(modelMat, glm::radians(-arm1Angle), {0.0f, 0.0f, 1.0f});
    modelMat =
        glm::rotate(modelMat, glm::radians(-arm2Angle), {0.0f, 0.0f, 1.0f});

    // Scale rope
    glm::mat4 ropeModelMat = modelMat;
    ropeModelMat =
        glm::translate(ropeModelMat, {0.0f, -ropeH / 2.0f * ropeScale, 0.0f});
    ropeModelMat = glm::scale(ropeModelMat, {0.0f, ropeScale, 0.0f});
    ropeModelMat = glm::translate(ropeModelMat, {0.0f, ropeH / 2.0f, 0.0f});

    rope.draw([&] {
      mvp = projectionMat * viewMat * ropeModelMat;
      glUniform3fv(colorId, 1, black);
      glUniformMatrix4fv(matrixIdOneColorShader, 1, GL_FALSE, &mvp[0][0]);
    });

    modelMat =
        glm::translate(modelMat, {0.0f, -ropeH * ropeScale - boxH, 0.0f});

    box.draw([&] {
      mvp = projectionMat * viewMat * modelMat;
      glUniform3fv(colorId, 1, red);
      glUniformMatrix4fv(matrixIdOneColorShader, 1, GL_FALSE, &mvp[0][0]);
    });

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  gizmo.destroy();
  base.destroy();
  arm1.destroy();
  arm2.destroy();
  box.destroy();
  rope.destroy();

  glfwTerminate();
}

/*
 * Use program
 * Bind VAO
 * Draw Array*/
// glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices2), vertices2);
