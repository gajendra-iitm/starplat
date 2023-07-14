from graphs.graph import DirGraph
import sys

# Declaring distance and visited dictionary
dist, visited = {}, {}

def update_dist(curr_node):

    # Getting neighbours of curr_node
    for edge in dg.edges:
        if edge.src == curr_node:
            dist[edge.dest] = min(dist[edge.dest], dist[curr_node] + edge.weight)


def get_min_unvisited(curr_node):
    minval, minnode = sys.maxsize, curr_node

    for node in visited:
        if not visited[node]:
            if dist[node] < minval:
                minval = dist[node]
                minnode = node

    return minnode


dg = DirGraph(r'/home/viswesh/Desktop/StarPlat/Py-Starplat/graphs/input.txt')


# Initializing distance and visited dictionary 
for node in dg.nodes:
    dist[node] = sys.maxsize
    visited[node] = False
dist[source_node] = 0
visited[source_node] = True

curr_node = source_node

for i in range(len(dg.nodes)):
    update_dist(curr_node)
    next_node = get_min_unvisited(curr_node)

    curr_node = next_node
    visited[curr_node] = True


# OUTPUT
for item in dist.items():
    print(item)