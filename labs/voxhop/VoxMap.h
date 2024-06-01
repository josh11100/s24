#ifndef VOXMAP_H
#define VOXMAP_H

#include <istream>
#include <vector>
#include <string>

#include "Point.h"
#include "Route.h"

class VoxMap {
  int width;
  int depth;
  int height;
  std::vector<std::vector<std::vector<bool>>> map;

  // Helper Functions
  void parseMap(std::istream& stream);
  bool isFilled(int x, int y, int z) const;
  bool isValidVoxel(int x, int y, int z) const;

public:
  VoxMap(std::istream& stream);
  ~VoxMap();

  Route route(Point src, Point dst);
};

#endif
