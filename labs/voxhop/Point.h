#ifndef POINT_H
#define POINT_H

#include <iostream>
#include <functional>
using namespace std;

struct Point {
  int x;
  int y;
  int z;

  Point() {}
  Point(int x, int y, int z): x(x), y(y), z(z) {}

  bool operator==(const Point& other) const {
    if(x == other.x && y == other.y && z == other.z) {
      return true;
    }
    return false; 

  }

  bool operator!=(const Point& other) const {
    return !(*this == other);
  }
  bool operator>(const Point& other) const {
    return !(*this > other);
  }
  bool operator<(const Point& other) const {
    return !(*this < other);
  }
};

std::istream& operator >> (std::istream& stream, Point& point);
std::ostream& operator << (std::ostream& stream, const Point& point);


struct PointHash {
  size_t operator()(const Point& pt) const {
    return hash<int>()(pt.x) + 31 * hash<int>()(pt.y) + 31 * 31 * hash<int>()(pt.z);
  }
};





#endif