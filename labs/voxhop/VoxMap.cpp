#include "VoxMap.h"
#include "Errors.h"
#include "Route.h"
#include <iostream>
#include <sstream>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <cmath>
#include <algorithm>
#include <set>
#include<map>


using namespace std;

VoxMap::VoxMap(std::istream& stream) {
    stream >> width >> depth >> height;
    map = vector<vector<vector<bool>>>(height, vector<vector<bool>>(depth, vector<bool>(width, false)));

    string tier;
    string binary;
    binary.reserve(width);  // Reserve memory for the binary string to avoid repeated allocations

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < depth; ++j) {
            getline(stream, tier);
            if (tier.empty()) {
                --j;  // Adjust for skipped empty line
                continue;
            }
            binary = conversion(tier);
            for (int k = 0; k < width; ++k) {
                map[i][j][k] = (binary[k] == '1');
            }
        }
    }
}
// VoxMap::VoxMap(std::istream& stream) {
//   stream >> width >> depth >> height;
//   map = vector<vector<vector<bool>>>(height, vector<vector<bool>>(depth, vector<bool>(width, false)));

//   string tier;
//   // string binary = "";
//   // i =z
//   // j = y 
//   // k = x
//   for(int i = 0; i < height; i++) {
//     for(int j = 0; j < depth; j++) {
//       getline(stream, tier);
//       if(tier == "") {
//         j--;
//         continue;
//       }
//       string binary = conversion(tier);
//       for(int k = 0; k < width; k++) {
//         if(binary[k] == '1') {
//           map[i][j][k] = true;  
//         }
//       }
     
//     }  // cout << "--------------" << endl;
//   }

  
// }



std::vector<Point> VoxMap::neighbors(const Point& pt) {
  std::vector<Point> nbs;
  std::vector<Point> directions = {
      {pt.x + 1, pt.y, pt.z},
      {pt.x - 1, pt.y, pt.z},
      {pt.x, pt.y + 1, pt.z},
      {pt.x, pt.y - 1, pt.z}
  };

  for(auto neighbor : directions) {
    if (!validBox(neighbor)) {
      continue;
    }

    if(neighbor.z > 0 && map[neighbor.z - 1][neighbor.y][neighbor.x] == false 
      && map[neighbor.z][neighbor.y][neighbor.x] == false) {
      while (neighbor.z > 0 && !map[neighbor.z - 1][neighbor.y][neighbor.x]) {
        neighbor.z--;
      }
    }


    if (Validity(neighbor)) {
      nbs.push_back(neighbor);
      continue;
    }

    if(neighbor.z > 0 && map[neighbor.z][neighbor.y][neighbor.x]) {
      neighbor.z++;
      if(neighbor.z < height && !map[neighbor.z][neighbor.y][neighbor.x] && !map[pt.z+1][pt.y][pt.x]) {
        nbs.push_back(neighbor);
        continue;
      }
    } 

  }

  return nbs;

}



VoxMap::~VoxMap() {
}


Route VoxMap::route(Point src, Point dst) {
  Route r;
  if(!val(src)) {
    throw InvalidPoint(src);
  }
  if(!val(dst)) {
    throw InvalidPoint(dst);
  }

  // vector<Point> s = neighbors(src);
  // for(size_t i = 0; i < s.size(); i++) {
  //   cout << s[i] << " ";
  // }
  // cout << endl;

  // try {
        // Attempt to find the route using Astart 
    // cout << "here before assignment" << endl;
    r = Astart(src, dst);
    // cout << "here" << endl;
    if(r.empty()) {
      // cout << "here" << endl;
      throw NoRoute(src, dst);
    }
    else {

      // cout << "hree at reutnr " << endl;
      return r;   
    }
        
    
    // } 
    // catch (const NoRoute&) {
        // If Astart throws a NoRoute exception, rethrow it again 
      // throw NoRoute(src, dst);
    // }

    // throw NoRoute(src, dst);
}

// scratcehd idea, too slow to pass the tests apparently 
// void VoxMap::BFS(const std::vector<std::vector<std::vector<bool>>>& map, const Point& src, const Point& dst) {
  
// }

Route VoxMap::Astart(const Point& src, const Point& dst) {
  std::unordered_set<Point, PointHash> cSet; // Closed set, already visited blocks | runtime: unordered > ordered 
  std::unordered_map<Point, Point, PointHash> mapPath; // Where it came from | runtime: unordered> ordered
  std::unordered_map<Point, double, PointHash> g, f; // Cost from start to block and total cost (g + heuristic) | runtieme: unorfdered > ordred 
  
  cSet.reserve(width * depth * height);
  mapPath.reserve(width * depth * height);
  g.reserve(width * depth * height);
  f.reserve(width * depth * height);
  
  Comparison comp(f);
  std::priority_queue<Point, std::vector<Point>, Comparison> oSet(comp); // the oopen set, blocks that need to be eval. | neeededs queeue []
  bool pathfound = false;
  Route path;
  // start at 0 and calc f with h func

   
  g[src] = 0;
  f[src] = h(src, dst);
// the h function also calculates the z coords bu ti dont think its necessary so take it out if it stil wokrs fine
  // cout << "here ins/tide" << endl;
   oSet.push(src);

  while(!oSet.empty()) {
    // cout << "inside hwile" << endl;
    Point currPoint = oSet.top();
    oSet.pop();

    if(currPoint == dst) {
      // cout << "here inside found dst" << endl;
      pathfound = true;

      while(currPoint != src) {
        //  cout << " insdie currP != src" << endl;
        // void findpath(mapPath[currPoint]);
        Point prev = mapPath[currPoint];

        if (prev.x < currPoint.x) {
          path.push_back(EAST);
        }
        else if (prev.x > currPoint.x) {
          path.push_back(WEST);
        }
        else if (prev.y < currPoint.y) {
          path.push_back(SOUTH);
        }
        else if (prev.y > currPoint.y) {
          path.push_back(NORTH);
        }
        currPoint = prev;
      }

      // TBA break for now;
      // path was found 
      break;
    }

    cSet.insert(currPoint);

    for(const auto& n : neighbors(currPoint)) {

      if(cSet.find(n) != cSet.end()) {
        continue;
      }
      // cout << "after" << endl;

      double gT = g[currPoint] + 1;
      if(!g.count(n) || gT < g[n]) {
        // cout << "here inside that if state ement \n";
        mapPath[n] = currPoint;
        g[n] = gT;
        f[n] = gT + h(n, dst);
        oSet.push(n);
      }
      // cout << "after second if statment " << endl;
    }
  }

  if(pathfound == false) {
    throw NoRoute(src, dst);
  }
  else {
    reverse(path.begin(), path.end());
    return path;
  }

}





double VoxMap::h(const Point& src, const Point& dst) {
  // returnt the f of f = h + g but 
  // double value;
  // value = abs(src.x - dst.x) + abs(src.y - dst.y);
  return abs(src.x - dst.x) + abs(src.y - dst.y);

  // return value;
}







// checks if the point is a valid point within the map created

bool VoxMap::Validity(const Point& point) {
  // bool check = true;
  if(point.x < 0 || point.x >= width || point.y < 0 || point.y >= depth || point.z <= 0 || point.z >= height) {
    // cout << "here1" << endl;
    return false;
  }
  if(map[point.z][point.y][point.x]) {
    // cout << "here2" << endl;
    return false;
  }
  // if(map[point.z][point.y][point.x] == false && map[point.z-1][point.y][point.x] == false) {
  //   // cout << "here2" << endl;
  //   return false;
  // }
  // if (point.z == 0 || !map[point.z -1][point.y][point.x]) {
  //   return false;
  // }
  return true;
}


bool VoxMap::val(const Point& point) {
  // bool check = true;
  if(point.x < 0 || point.x >= width || point.y < 0 || point.y >= depth || point.z <= 0 || point.z >= height) {
    // cout << "here1" << endl;
    return false;
  }
  if(map[point.z][point.y][point.x]) {
    // cout << "here2" << endl;
    return false;
  }
  if(map[point.z][point.y][point.x] == false && map[point.z-1][point.y][point.x] == false) {
    // cout << "here2" << endl;
    return false;
  }
  // if (point.z == 0 || !map[point.z -1][point.y][point.x]) {
  //   return false;
  // }
  return true;
}



// valid box within map (can be true | falsw)
bool VoxMap::validBox(const Point& point) {
  if (point.x < 0 || point.x >= width || point.y < 0 || point.y >= depth || point.z <= 0 || point.z >= height) {
    return false;
  }
  return true;
}




// std::string VoxMap::conversion(const std::string& str) {
//     static const std::unordered_map<char, std::string> dict = {
//       {'0', "0000"}, {'1', "0001"}, {'2', "0010"}, {'3', "0011"},
//       {'4', "0100"}, {'5', "0101"}, {'6', "0110"}, {'7', "0111"},
//       {'8', "1000"}, {'9', "1001"}, {'a', "1010"}, {'b', "1011"},
//       {'c', "1100"}, {'d', "1101"}, {'e', "1110"}, {'f', "1111"},
//       {'A', "1010"}, {'B', "1011"}, {'C', "1100"}, {'D', "1101"},
//       {'E', "1110"}, {'F', "1111"}
//     };
    
//     std::string binaryStr;
//     binaryStr.reserve(str.size() * 4); // Reserve memory to avoid multiple allocations

//     for (char ch : str) {
//       binaryStr.append(dict.at(ch));
//     }
    
//     return binaryStr;
// }




// // converts from hex to binary
std::string VoxMap::conversion(const std::string& str) {
  std::string binaryStr = "";
  for (size_t i = 0; i < str.size(); ++i) {
    switch (str[i]) {
      case '0': 
        binaryStr.append("0000"); 
        break;
      case '1': 
        binaryStr.append("0001"); 
        break;
      case '2': 
        binaryStr.append("0010"); 
        break;
      case '3': 
        binaryStr.append("0011"); 
        break;
      case '4': 
        binaryStr.append("0100"); 
        break;
      case '5': 
        binaryStr.append("0101"); 
        break;
      case '6': 
        binaryStr.append("0110"); 
        break;
      case '7': 
        binaryStr.append("0111"); 
        break;
      case '8': 
        binaryStr.append("1000"); 
        break;
      case '9': 
        binaryStr.append("1001"); 
        break;
      case 'a':   
        binaryStr.append("1010"); 
        break;
      case 'b': 
        binaryStr.append("1011"); 
        break;
      case 'c': 
        binaryStr.append("1100"); 
        break;
      case 'd': 
        binaryStr.append("1101"); 
        break;
      case 'e': 
        binaryStr.append("1110"); 
        break;
      case 'f': 
        binaryStr.append("1111"); 
        break;
    }
  }
  return binaryStr;
}