#include "camera.hpp"

Camera::Camera(GLFWwindow *window, float speed, float mouseSpeed) {
  this->window = window;
  this->speed = speed;
  this->mouseSpeed = mouseSpeed;
}

void Camera::computeMatrices(float dt) {

  // Get mouse position
  double xpos, ypos;
  glfwGetCursorPos(window, &xpos, &ypos);

  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  // Reset mouse position for next frame
  glfwSetCursorPos(window, width / 2.0, height / 2.0);

  // Compute new orientation
  horizontalAngle += mouseSpeed * float(width / 2.0 - xpos);
  verticalAngle += mouseSpeed * float(height / 2.0 - ypos);

  // Direction : Spherical coordinates to Cartesian coordinates conversion
  glm::vec3 direction(cos(verticalAngle) * sin(horizontalAngle),
                      sin(verticalAngle),
                      cos(verticalAngle) * cos(horizontalAngle));

  // Right vector
  glm::vec3 right = glm::vec3(sin(horizontalAngle - 3.14f / 2.0f), 0,
                              cos(horizontalAngle - 3.14f / 2.0f));

  // Up vector
  glm::vec3 up = glm::cross(right, direction);

  // Speed multiplier
  float mult = 1.0f;
  if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
    mult = 0.15f;
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
    mult = 3.0f;
  }

  // Move forward
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    position += direction * dt * speed * mult;
  }
  // Move backward
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    position -= direction * dt * speed * mult;
  }
  // Strafe right
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    position += right * dt * speed * mult;
  }
  // Strafe left
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    position -= right * dt * speed * mult;
  }

  projectionMatrix = glm::perspective(glm::radians(45.0f),
                                      float(width) / height, 0.1f, 100000.0f);
  viewMatrix = glm::lookAt(position, position + direction, up);
}