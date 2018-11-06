#pragma once
#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <numeric>
#include <glm/glm.hpp>
using namespace std;

namespace mesh {

struct FileData {
  float xStep;
  float zStep;
  vector<vector<float>> yVals;
};

vector<glm::vec3> genMesh(const vector<vector<float>> &yVals, float xStep, float zStep) {
  vector<glm::vec3> points;
  points.reserve(yVals.size() * yVals.at(0).size());
  float currentZ = 0.0f;
  for (const auto &row : yVals) {
    float currentX = 0.0f;
    for (const float currentY : row) {
      points.emplace_back(currentX, currentY, currentZ);
      currentX += xStep;      
    }
    currentZ += zStep;
  }
  return points;
}

vector<unsigned int> genIndices(unsigned int xTotal, unsigned int zTotal) {
  /* In a regular mesh:
   * A is for the evens points rows.
   * B is for the odds points rows.
   * last1 and last2 are the last indices of the last triangle.
   */

  vector<unsigned int> indices(3 * (xTotal - 1) * 2 * (zTotal - 1));
  size_t idx = 0;
  for (unsigned int i = 0; i < zTotal - 1; i++) {
    unsigned int idxA = i * xTotal, idxB = (i + 1) * xTotal;
    unsigned int last1 = idxA, last2 = idxB;
    for (unsigned int j = 0 ; j < xTotal - 1; j++) {
      idxA++;
      idxB++;

      indices[idx] = last1;
      indices[idx + 1] = idxB;
      indices[idx + 2] = last2;

      last2 = idxA;

      indices[idx + 3] = last1;
      indices[idx + 4] = last2;
      indices[idx + 5] = idxB;
      idx += 6;

      last1 = last2;
      last2 = idxB;
    }
  }

  return indices;
}

vector<glm::vec3> genNormals(const vector<unsigned int> &indices, const vector<glm::vec3> &points) {
  vector<vector<glm::vec3>> trianglesNors(points.size(), vector<glm::vec3>());
  vector<glm::vec3> vertexNors(points.size(), glm::vec3());

  for (size_t i = 0; i < indices.size(); i += 3) {
    const glm::vec3 &first = points[indices[i]];
    const glm::vec3 &second = points[indices[i + 1]];
    const glm::vec3 &third = points[indices[i + 2]];
    const glm::vec3 one = second - first;
    const glm::vec3 two = third - first;
    const glm::vec3 norm = glm::normalize(glm::cross(one,two));

    trianglesNors[indices[i]].push_back(norm);
    trianglesNors[indices[i + 1]].push_back(norm);
    trianglesNors[indices[i + 2]].push_back(norm);
  }

  for (size_t i = 0; i < vertexNors.size(); i++) {
    vertexNors[i] = accumulate(trianglesNors[i].begin(), trianglesNors[i].end(),
                               glm::vec3(0));
    vertexNors[i] /= trianglesNors[i].size();
  }

  return vertexNors;
}

vector<glm::vec2> genTexCoords(unsigned int xTotal, unsigned int zTotal) {
  vector<glm::vec2> texCoords;
  texCoords.reserve(xTotal * zTotal);
  float sStep = 1.0f/float(xTotal - 1);
  float tStep = 1.0f/float(zTotal - 1);

  for (float s = 0.0f; s <= 1.0f; s += sStep) {
    for (float t = 0.0f; t <= 1.0f; t += tStep) {
      texCoords.emplace_back(s,t);
    }
  }

  return texCoords;
}

FileData read(const string &path) {
  ifstream file(path);
  if (!file.is_open()) {
    cout << "Can not read \"" << path << "\"" << endl;
    return FileData();
  }

  FileData data;

  // Read the first line: metadata.
  string line;
  getline(file, line);
  stringstream strStream(line);
  string garbage;
  for (int i = 0; i < 7; i++) {
    strStream >> garbage;
  }
  // points in X, points in Z.
  int cols, rows;

  try {
    strStream >> data.xStep >> data.zStep >> cols >> rows;
  } catch (const std::exception &e) {
    cout << "Something gone wrong! :(" << endl;
    cout << e.what() << endl;
    return FileData();
  }

  // Read points.
  data.yVals =  vector<vector<float>>(rows, vector<float>(cols, 0.0f));

  int lineCount = 0;
  while (lineCount < rows and getline(file, line)) {
    strStream = stringstream(line);
    for (int i = 0; i < cols; i++) {
      strStream >> data.yVals[lineCount][i];
    }
    lineCount++;
  }

  file.close();

  return data;
}

} // namespace file