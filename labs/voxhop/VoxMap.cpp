#include "VoxMap.h"
#include "Errors.h"
#include <sstream>
#include <bitset>
#include <stdexcept>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <iostream>
#include <fstream>

struct PointHash {
    std::size_t operator()(const Point& point) const {
        return std::hash<int>()(point.x) ^ std::hash<int>()(point.y) ^ std::hash<int>()(point.z);
    }
};

VoxMap::VoxMap(std::istream& stream) {
    parseMap(stream);
}

VoxMap::~VoxMap() {
    // No dynamic memory to clean up
}

void VoxMap::parseMap(std::istream& stream) {
    stream >> width >> depth >> height;
    if (stream.fail() || width <= 0 || depth <= 0 || height <= 0) {
        throw std::invalid_argument("Invalid map dimensions");
    }

    map.resize(height, std::vector<std::vector<bool>>(depth, std::vector<bool>(width, false)));

    std::string line;
    std::getline(stream, line); // Skip the newline character after dimensions

    for (int z = 0; z < height; ++z) {
        if (!std::getline(stream, line)) {
            throw std::invalid_argument("Unexpected end of file while reading map data (missing empty line)");
        }
        if (!line.empty()) {
            throw std::invalid_argument("Expected empty line between tiers, found: " + line);
        }

        for (int y = 0; y < depth; ++y) {
            if (!std::getline(stream, line)) {
                throw std::invalid_argument("Unexpected end of file while reading map data");
            }

            if (line.length() != static_cast<size_t>(width / 4)) {
                std::ostringstream oss;
                oss << "Invalid line length in map file: expected " << (width / 4) << " but found " << line.length();
                throw std::invalid_argument(oss.str());
            }

            for (int x = 0; x < width / 4; ++x) {
                char hexDigit = line[x];
                if (!std::isxdigit(hexDigit)) {
                    throw std::invalid_argument("Invalid character in map file: " + std::string(1, hexDigit));
                }
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
    if (x < 0 || x >= width || y < 0 || y >= depth || z < 0 || z >= height) {
        return true; // Out of bounds is considered filled
    }
    return map[z][y][x];
}

bool VoxMap::isValidVoxel(int x, int y, int z) const {
    if (x < 0 || x >= width || y < 0 || y >= depth || z < 0 || z >= height) {
        return false;
    }
    return !isFilled(x, y, z) && (z == 0 || isFilled(x, y, z - 1));
}

bool VoxMap::isEmptyAbove(int x, int y, int z) const {
    return !isFilled(x, y, z + 1);
}

std::vector<Point> VoxMap::getNeighbors(const Point& point) const {
    std::vector<Point> neighbors;
    static const std::vector<std::pair<int, int>> directions = {
        {0, 1}, {1, 0}, {0, -1}, {-1, 0}
    };

    for (const auto& [dx, dy] : directions) {
        int nx = point.x + dx;
        int ny = point.y + dy;
        int nz = point.z;

        // Ensure new position is within bounds and valid
        if (nx >= 0 && nx < width && ny >= 0 && ny < depth) {
            // Horizontal move
            if (isValidVoxel(nx, ny, nz) && isEmptyAbove(nx, ny, nz)) {
                neighbors.emplace_back(nx, ny, nz);
            }

            // Fall
            int downZ = nz;
            while (downZ > 0 && !isFilled(nx, ny, downZ - 1)) {
                downZ--;
            }
            if (downZ != nz && isValidVoxel(nx, ny, downZ)) {
                neighbors.emplace_back(nx, ny, downZ);
            }

            // Jump
            if (nz + 1 < height && isValidVoxel(nx, ny, nz + 1) && isEmptyAbove(nx, ny, nz + 1) && isEmptyAbove(nx, ny, nz + 2)) {
                neighbors.emplace_back(nx, ny, nz + 1);
            }
        }
    }

    return neighbors;
}

int VoxMap::heuristic(const Point& a, const Point& b) const {
    return abs(a.x - b.x) + abs(a.y - b.y) + abs(a.z - b.z);
}

Route VoxMap::route(Point src, Point dst) {
    if (!isValidVoxel(src.x, src.y, src.z)) {
        throw InvalidPoint(src);
    }
    if (!isValidVoxel(dst.x, dst.y, dst.z)) {
        throw InvalidPoint(dst);
    }

    std::priority_queue<std::pair<int, Point>, std::vector<std::pair<int, Point>>, std::greater<>> frontier;
    frontier.emplace(0, src);

    std::unordered_map<Point, Point, PointHash> cameFrom;
    std::unordered_map<Point, int, PointHash> costSoFar;
    cameFrom[src] = src;
    costSoFar[src] = 0;

    while (!frontier.empty()) {
        Point current = frontier.top().second;
        frontier.pop();

        if (current == dst) {
            Route path;
            Point step = current;
            while (step != src) {
                Point prev = cameFrom[step];
                if (prev.x < step.x) path.push_back(Move::EAST);
                else if (prev.x > step.x) path.push_back(Move::WEST);
                else if (prev.y < step.y) path.push_back(Move::SOUTH);
                else if (prev.y > step.y) path.push_back(Move::NORTH);
                step = prev;
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        for (const Point& next : getNeighbors(current)) {
            int newCost = costSoFar[current] + 1;
            if (costSoFar.find(next) == costSoFar.end() || newCost < costSoFar[next]) {
                costSoFar[next] = newCost;
                int priority = newCost + heuristic(next, dst);
                frontier.emplace(priority, next);
                cameFrom[next] = current;
            }
        }
    }

    throw NoRoute(src, dst);
}
