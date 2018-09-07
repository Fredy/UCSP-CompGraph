#version 330 core

layout (location = 0) in vec3 aPos;
uniform vec2 windowSzie;
void main() {
  gl_Position = vec4(aPos.x / 800.0f , aPos.y / 600.0f, aPos.z, 1.0);
}