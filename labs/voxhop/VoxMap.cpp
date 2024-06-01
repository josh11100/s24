#include "VoxMap.h"
#include "Errors.h"
#include <sstream>
#include <bitset>

VoxMap::VoxMap(std::istream& stream) {
  parseMap(stream);
}

VoxMap::~VoxMap() {
  // No dynamic memory to clean up
}

void VoxMap::parseMap(std::istream& stream) {
  stream >> width >> depth >> height;
  map.resize(height, std::vector<std::vector<bool>>(depth, std::vector<bool>(width, false)));

  std::string line;
  for (int z = 0; z < height; ++z) {
    std::getline(stream, line); // Skip the empty line
    for (int y = 0; y < depth; ++y) {
      std::getline(stream, line);
      for (int x = 0; x < width / 4; ++x) {
        char hexDigit = line[x];
        int value = std::stoi(std::string(1, hexDigit), nullptr, 16);
        std::bitset<4> bits(value);
        for (int bit = 0; bit < 4; ++bit) {
          map[z][y][x * 4 + bit] = bits[3 - bit]; // High bit is westmost
        }
      }
    }
  }
}

bool VoxMap::isFilled(int x, int y, int z) const {
  return map[z][y][x];
}

bool VoxMap::isValidVoxel(int x, int y, int z) const {
  return !isFilled(x, y, z) && isFilled(x, y, z - 1);
}

Route VoxMap::route(Point src, Point dst) {
  if (!isValidVoxel(src.x, src.y, src.z)) {
    throw InvalidPoint(src);
  }
  if (!isValidVoxel(dst.x, dst.y, dst.z)) {
    throw InvalidPoint(dst);
  }
  throw NoRoute(src, dst); // Placeholder until pathfinding is implemented
}
