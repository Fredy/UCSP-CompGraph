#version 330 core

out vec4 FragColor;
in vec3 outColor;

void main() {
  if(gl_FrontFacing) {
    FragColor = vec4(outColor, 1.0);
  } else {
    FragColor = vec4(vec3(1.0, 1.0, 1.0) - outColor, 1.0);
  }
}