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

using namespace std;

VoxMap::VoxMap(std::istream& stream) {
    stream >> width >> depth >> height;
    map = vector<vector<vector<bool>>>(height, vector<vector<bool>>(depth, vector<bool>(width, false)));

    string tier;
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < depth; ++j) {
            getline(stream, tier);
            if (tier.empty()) {
                --j;  // Adjust for skipped empty line
                continue;
            }
            string binary = conversion(tier);
            for (int k = 0; k < width; ++k) {
                map[i][j][k] = (binary[k] == '1');
            }
        }
    }
}

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

        if(neighbor.z > 0 && !map[neighbor.z - 1][neighbor.y][neighbor.x] 
            && !map[neighbor.z][neighbor.y][neighbor.x]) {
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

    try {
        r = Astart(src, dst);
        if(r.empty()) {
            throw NoRoute(src, dst);
        }
        else {
            return r;   
        }
    } 
    catch (const NoRoute&) {
        throw NoRoute(src, dst);
    }
}

Route VoxMap::Astart(const Point& src, const Point& dst) {
    std::unordered_set<Point, PointHash> cSet; 
    std::unordered_map<Point, Point, PointHash> mapPath; 
    std::unordered_map<Point, double, PointHash> g, f; 
    
    cSet.reserve(width * depth * height);
    mapPath.reserve(width * depth * height);
    g.reserve(width * depth * height);
    f.reserve(width * depth * height);
    
    Comparison comp(f);
    std::priority_queue<Point, std::vector<Point>, Comparison> oSet(comp);
    bool pathfound = false;
    Route path;
    
    g[src] = 0;
    f[src] = h(src, dst);

    oSet.push(src);

    while(!oSet.empty()) {
        Point currPoint = oSet.top();
        oSet.pop();

        if(currPoint == dst) {
            pathfound = true;

            while(currPoint != src) {
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
            break;
        }

        cSet.insert(currPoint);

        for(const auto& n : neighbors(currPoint)) {
            if(cSet.find(n) != cSet.end()) {
                continue;
            }

            double gT = g[currPoint] + 1;
            if(!g.count(n) || gT < g[n]) {
                mapPath[n] = currPoint;
                g[n] = gT;
                f[n] = gT + h(n, dst);
                oSet.push(n);
            }
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
    return abs(src.x - dst.x) + abs(src.y - dst.y);
}

bool VoxMap::Validity(const Point& point) {
    if(point.x < 0 || point.x >= width || point.y < 0 || point.y >= depth || point.z <= 0 || point.z >= height) {
        return false;
    }
    if(map[point.z][point.y][point.x]) {
        return false;
    }
    return true;
}

bool VoxMap::val(const Point& point) {
    if(point.x < 0 || point.x >= width || point.y < 0 || point.y >= depth || point.z <= 0 || point.z >= height) {
        return false;
    }
    if(map[point.z][point.y][point.x]) {
        return false;
    }
    if(!map[point.z-1][point.y][point.x]) {
        return false;
    }
    return true;
}

bool VoxMap::validBox(const Point& point) {
    if (point.x < 0 || point.x >= width || point.y < 0 || point.y >= depth || point.z <= 0 || point.z >= height) {
        return false;
    }
    return true;
}

std::string VoxMap::conversion(const std::string& str) {
    static const std::unordered_map<char, std::string> dict = {
        {'0', "0000"}, {'1', "0001"}, {'2', "0010"}, {'3', "0011"},
        {'4', "0100"}, {'5', "0101"}, {'6', "0110"}, {'7', "0111"},
        {'8', "1000"}, {'9', "1001"}, {'a', "1010"}, {'b', "1011"},
        {'c', "1100"}, {'d', "1101"}, {'e', "1110"}, {'f', "1111"},
        {'A', "1010"}, {'B', "1011"}, {'C', "1100"}, {'D', "1101"},
        {'E', "1110"}, {'F', "1111"}
    };
    
    std::string binaryStr;
    binaryStr.reserve(str.size() * 4); 

    for (char ch : str) {
        binaryStr.append(dict.at(ch));
    }
    
    return binaryStr;
}