#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 normals;
layout (location = 0) in vec3 texCoords;
uniform mat4 mvp;

void main() {
  gl_Position = mvp * vec4(aPos, 1.0);
}

