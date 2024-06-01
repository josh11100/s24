#ifndef POINT_H
#define POINT_H

#include <iostream>

struct Point {
  int x;
  int y;
  int z;

  Point() {}
  Point(int x, int y, int z): x(x), y(y), z(z) {}

  // Equality operator
  bool operator==(const Point& other) const {
    return x == other.x && y == other.y && z == other.z;
  }

  // Less than operator
  bool operator<(const Point& other) const {
    if (x != other.x) return x < other.x;
    if (y != other.y) return y < other.y;
    return z < other.z;
  }
};

std::istream& operator>>(std::istream& stream, Point& point);
std::ostream& operator<<(std::ostream& stream, const Point& point);

#endif
