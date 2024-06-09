
#ifndef VOXMAP_H
#define VOXMAP_H

#include <istream>
#include <vector>
#include <string>
#include <unordered_map>
#include <map>


#include "Point.h"
#include "Route.h"

using namespace std;

class VoxMap {
  // Point bounds;
  // std::vector<int> xdir;
  // std::vectr<int> ydir;
  // std::vector<int> zdir;



  int width; // this is x
  int depth; // this is y
  int height; // this is z

  std::vector<vector<vector<bool>>> map;

  std::vector<Point> nbs;
  
  
  // Member Variables


  // Helper Functions
  // convert hex to binary 
  std::string conversion(const std::string& str);


  // void BFS(const std::vector<std::vector<std::vector<bool>>>& map, const Point& src, const Point& dst);
  std::vector<Point> neighbors(const Point& pt);

  
  // do this after i do the BFS
  //
  bool Validity(const Point& point);
  bool val(const Point& point);

  bool validBox(const Point& point);

 Route Astart(const Point& src, const Point& dst);



  // bool fullBox(const Point& point);

  double h(const Point& src, const Point& dst);
  

 struct Comparison {
    std::unordered_map<Point, double, PointHash>& fScore;

    Comparison(std::unordered_map<Point, double, PointHash>& fScore) : fScore(fScore) {}

    bool operator()(const Point& lhs, const Point& rhs) const {
      return fScore[lhs] > fScore[rhs];
    }
  };
 

public:
  VoxMap(std::istream& stream);
  ~VoxMap();

  Route route(Point src, Point dst);
};

#endif
