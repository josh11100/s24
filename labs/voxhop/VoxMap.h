#ifndef VOXMAP_H
#define VOXMAP_H

#include <istream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>

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
  std::vector<Point> getNeighbors(const Point& point) const;
  int heuristic(const Point& a, const Point& b) const;

public:
  VoxMap(std::istream& stream);
  ~VoxMap();

  Route route(Point src, Point dst);
  bool isValidPoint(const Point& point) const; // Add this function to the public interface
};

#endif // VOXMAP_H
