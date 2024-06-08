from collections import deque

def is_valid(x, y, z, voxel_map, width, depth, height):
    return 0 <= x < width and 0 <= y < depth and 0 <= z < height and voxel_map[x][y][z] == 0

def bfs(start, goal, voxel_map, width, depth, height):
    queue = deque([start])
    visited = set()
    visited.add(start)

    while queue:
        x, y, z = queue.popleft()
        
        if (x, y, z) == goal:
            return True

        # Check all 6 possible movements (left, right, forward, backward, up, down)
        directions = [(-1, 0, 0), (1, 0, 0), (0, -1, 0), (0, 1, 0), (0, 0, -1), (0, 0, 1)]
        for dx, dy, dz in directions:
            nx, ny, nz = x + dx, y + dy, z + dz
            if is_valid(nx, ny, nz, voxel_map, width, depth, height) and (nx, ny, nz) not in visited:
                queue.append((nx, ny, nz))
                visited.add((nx, ny, nz))
    
    return False

# Define the voxel map dimensions
width, depth, height = 12, 12, 7

# Create the voxel map (initialize with all 0s)
voxel_map = [[[0 for _ in range(height)] for _ in range(depth)] for _ in range(width)]

# Fill in the voxel map with provided values (this is truncated for simplicity)
voxels = [
    (0, 0, 0), (1, 0, 0), (2, 0, 0), (3, 0, 0), (4, 0, 0), (5, 0, 0), (6, 0, 0), (7, 0, 0), (8, 0, 0), (9, 0, 0), (10, 0, 0), (11, 0, 0),
    # ... (include all the other provided voxel data here)
    # Example (0, 0, 6) = 1 means voxel_map[0][0][6] = 1
]

# Populate the voxel map
for x, y, z in voxels:
    voxel_map[x][y][z] = 1

# Define the test routes
test_routes = [
    ((0, 0, 6), (11, 0, 6)),
    ((11, 0, 6), (11, 11, 6)),
    ((11, 11, 6), (0, 11, 6)),
    ((0, 11, 6), (0, 0, 6)),
    ((6, 11, 6), (4, 9, 2)),
    ((10, 9, 4), (8, 11, 6)),
    ((3, 3, 3), (5, 6, 1)),
    ((3, 2, 4), (1, 1, 5)),
    ((8, 4, 3), (6, 0, 1))
]

# Check and print results for each route
for start, goal in test_routes:
    if bfs(start, goal, voxel_map, width, depth, height):
        print(f"Valid route from {start} to {goal}.")
    else:
        print(f"No route from {start} to {goal}.")
