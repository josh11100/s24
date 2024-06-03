#include "VoxMap.h"
#include "Errors.h"

#include <fstream>
#include <iostream>

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "USAGE: " << argv[0] << " [map-file]\n";
    return 1;
  }

  std::ifstream stream(argv[1]);
  if (stream.fail()) {
    std::cerr << "ERROR: Could not open file: " << argv[1] << '\n';
    return 1;
  }

  VoxMap map(stream);
  stream.close();

  Point src;
  Point dst;
  while (std::cin >> src >> dst) {
    try {
      // Check if the source and destination points are valid before finding a route
      if (!map.isValidPoint(src)) {
        throw InvalidPoint(src);
      }
      if (!map.isValidPoint(dst)) {
        throw InvalidPoint(dst);
      }
      Route route = map.route(src, dst);
      std::cout << route << '\n';
    } catch (const InvalidPoint& err) {
      std::cout << "Invalid point: " << err.point() << '\n';
    } catch (const NoRoute& err) {
      std::cout << "No route from " << err.src() << " to " << err.dst() << ".\n";
    }
  }

  return 0;
}
