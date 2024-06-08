#ifndef VOXMAP_H
#define VOXMAP_H

#include <vector>
#include <iostream>
#include "Point.h"
#include "Route.h"
#include "Errors.h"

class VoxMap {
public:
    VoxMap(std::istream& stream);
    ~VoxMap();
    
    Route route(Point src, Point dst);

private:
    int width, depth, height;
    std::vector<std::vector<std::vector<bool>>> map;

    void parseMap(std::istream& stream);
    bool isFilled(int x, int y, int z) const;
    bool isValidVoxel(int x, int y, int z) const;
    bool isEmptyAbove(int x, int y, int z) const;
    std::vector<Point> getNeighbors(const Point& point) const;
    int heuristic(const Point& a, const Point& b) const;
};

#endif // VOXMAP_H
