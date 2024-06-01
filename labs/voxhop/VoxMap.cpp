#include "VoxMap.h"
#include "Errors.h"
#include <sstream>
#include <bitset>
#include <stdexcept>

VoxMap::VoxMap(std::istream& stream) {
    try {
        parseMap(stream);
    } catch (const std::exception& e) {
        std::cerr << "Error while parsing map: " << e.what() << std::endl;
        throw;
    }
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
    for (int z = 0; z < height; ++z) {
        std::getline(stream, line); // Skip the empty line
        if (!line.empty()) {
            throw std::invalid_argument("Expected empty line between tiers");
        }
        for (int y = 0; y < depth; ++y) {
            if (!std::getline(stream, line) || line.length() != static_cast<size_t>(width / 4)) {
                throw std::invalid_argument("Invalid line length in map file");
            }
            for (int x = 0; x < width / 4; ++x) {
                char hexDigit = line[x];
                if (!std::isxdigit(hexDigit)) {
                    throw std::invalid_argument("Invalid character in map file");
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
    return map[z][y][x];
}

bool VoxMap::isValidVoxel(int x, int y, int z) const {
    if (x < 0 || x >= width || y < 0 || y >= depth || z <= 0 || z >= height) {
        return false;
    }
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
