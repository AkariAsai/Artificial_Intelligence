from collections import deque
from heapq import heappop, heappush


def solve_bfs(maze, graph):
    start, goal = (1, 1), (len(maze) - 2, len(maze[0]) - 2)
    queue = deque([("", start)])
    visited = set()
    visited_node_num = 0
    while queue:
        path, current = queue.popleft()
        visited_node_num +=1
        if current == goal:
            print("visited node num is " + str(visited_node_num))
            return path
        if current in visited:
            continue
        visited.add(current)
        for direction, neighbour in graph[current]:
            queue.append((path + direction, neighbour))
    return "Cannot find any way."


def solve_dfs(maze, graph):
    start, goal = (1, 1), (len(maze) - 2, len(maze[0]) - 2)
    stack = deque([("", start)])
    visited = set()
    visited_node_num = 0
    while stack:
        path, current = stack.pop()
        visited_node_num +=1
        if current == goal:
            print("visited node num is " + str(visited_node_num))
            return path
        if current in visited:
            continue
        visited.add(current)
        for direction, neighbour in graph[current]:
            stack.append((path + direction, neighbour))
    return "Cannot find any way."


# Calculate a distance from the current cell to the goal, which is
# heuristic value for a-star search
def heuristic(cell, goal):
    return abs(cell[0] - goal[0]) + abs(cell[1] - goal[1])


def solve_astar(maze, graph):
    start, goal = (1, 1), (len(maze) - 2, len(maze[0]) - 2)
    pr_queue = []
    heappush(pr_queue, (0 + heuristic(start, goal), 0, "", start))
    visited = set()
    visited_node_num = 0
    while pr_queue:
        _, cost, path, current = heappop(pr_queue)
        visited_node_num +=1
        if current == goal:
            print("visited node num is " + str(visited_node_num))
            return path
        if current in visited:
            continue
        visited.add(current)
        for direction, neighbour in graph[current]:
            heappush(pr_queue, (cost + heuristic(neighbour, goal), cost + 1,
                                path + direction, neighbour))
    return "Cannot find any way."
