#include "texture.hpp"
#include <glad/glad.h>
#include <iostream>
#include <stb/stb_image.h>

namespace texture {

unsigned int load(const string &path) {
  unsigned int textureId;
  glGenTextures(1, &textureId);
  glBindTexture(GL_TEXTURE_2D, textureId);
  // set the texture wrapping/filtering options (on the currently bound texture
  // object)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  // load and generate the texture
  int width, height, nrChannels;
  unsigned char *data =
      stbi_load(path.data(), &width, &height, &nrChannels, 0);
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, data);
  } else {
    cout << "Failed to load texture: " << path << endl;
    glDeleteTextures(1, &textureId);
    textureId = 0;
  }
  stbi_image_free(data);

  return textureId;
}

void free(unsigned int textureId) { glDeleteTextures(1, &textureId); }

} // namespace texture
