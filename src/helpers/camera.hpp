#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
private:
  glm::mat4 viewMatrix;
  glm::mat4 projectionMatrix;

  GLFWwindow *window;

  // Initial position : on +Z
  glm::vec3 position = glm::vec3(0, 0, 5);
  // Initial horizontal angle : toward -Z
  float horizontalAngle = 3.14f;
  // Initial vertical angle : none
  float verticalAngle = 0.0f;

  float speed = 30.0f;
  float mouseSpeed = 0.0025f;

public:
  Camera(GLFWwindow *window, float speed = 30.0f, float mouseSpeed = 0.0025f);

  void computeMatrices(float dt);

  const glm::mat4 &getViewMatrix() const { return viewMatrix; }
  const glm::mat4 &getProjectionMatrix() const { return projectionMatrix; }
};