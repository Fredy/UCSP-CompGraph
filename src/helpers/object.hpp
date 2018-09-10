#pragma once

#include "glad/glad.h"

class Object {
private:
  int numVertices;
  GLuint vao;
  GLuint vbo[2];
  GLuint shaderId;
  GLenum mode;

public:
  void setVerticesData(int numVertices, const GLvoid *points, const GLvoid *colors = nullptr) {
    this->numVertices = numVertices;

    glGenVertexArrays(1, &vao);
    glGenBuffers(2, vbo);

    glBindVertexArray(vao);
    // Points
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, 3* numVertices * sizeof(GLfloat), points,
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
                          (void *)0);
    glEnableVertexAttribArray(0);
    // Colors
    if (colors != nullptr) {
      glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
      glBufferData(GL_ARRAY_BUFFER, 3* numVertices * sizeof(GLfloat), colors,
                   GL_STATIC_DRAW);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
                            (void *)0);
      glEnableVertexAttribArray(1);
    }
    // Unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }

  void setShader(GLuint shaderId) {
    this->shaderId = shaderId;
  }

  void setMode(GLenum mode) {
    this->mode = mode;
  }

  template<typename Functor>
  void draw(Functor && mvpModifications) {
    glUseProgram(shaderId);
    glBindVertexArray(vao);
    mvpModifications();
    glDrawArrays(mode, 0, numVertices);
    glBindVertexArray(0);
    glUseProgram(0);
  }

  void destroy() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(2, vbo);
  }

  void setExternalData(GLuint vao, GLuint vbo1, GLuint vbo2, int size) {
    this->vao =  vao;
    this->vbo[0] = vbo1;
    this->vbo[1] = vbo2;
    numVertices = size;
  }
};