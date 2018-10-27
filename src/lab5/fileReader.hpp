#pragma once
#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

namespace file {
void genMesh(const vector<vector<float>> &yVals, float xStep, float zStep) {}

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
      indices[idx + 1] = last2;
      indices[idx + 2] = idxB;

      last1 = last2;
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

void read(const string &path) {
  ifstream file(path);
  if (!file.is_open()) {
    cout << "Can not read \"" << path << "\"" << endl;
    return;
  }
  // Read the first line: metadata.
  string line;
  getline(file, line);
  stringstream strStream(line);
  string garbage;
  for (int i = 0; i < 7; i++) {
    strStream >> garbage;
  }
  float xStep, zStep;
  // points in X, points in Z.
  int cols, rows;

  try {
    strStream >> xStep >> zStep >> cols >> rows;
  } catch (const std::exception &e) {
    cout << "Something gone wrong! :(" << endl;
    cout << e.what() << endl;
    return;
  }

  // Read points.
  vector<vector<float>> yVals(rows, vector<float>(cols, 0.0f));

  int lineCount = 0;
  while (lineCount < rows and getline(file, line)) {
    strStream = stringstream(line);
    for (int i = 0; i < cols; i++) {
      strStream >> yVals[lineCount][i];
    }
    lineCount++;
  }

  file.close();
}

} // namespace file