#pragma once
#include <string>
using namespace std;

namespace texture {

unsigned int load(const string &path);
void free(unsigned int textureId);

} // namespace texture
