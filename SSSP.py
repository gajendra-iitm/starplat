from graphs.graph import UndirGraph
import sys

# Declaring distance and visited dictionary
dist, visited = {}, {}

def update_dist(curr_node):

    # Getting neighbours of curr_node
    for edge in ug.edges:
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


ug = UndirGraph()
nodes = list(map(int, input('Enter nodes: ').split()))
ug.add_nodes(*nodes)
# Ex: 5 6 7 8

edges = eval(input('Enter edges and weights:(nested tuples)\n'))
ug.add_edges(*edges)
# Ex: ( (5, 6, 1), (6, 8, 4), )


source_node = int(input("Enter source node: "))
if source_node not in ug.nodes:
    print("Node doesn't exist")
    exit(1)


# Initializing distance and visited dictionary 
for node in ug.nodes:
    dist[node] = sys.maxsize
    visited[node] = False
dist[source_node] = 0
visited[source_node] = True

curr_node = source_node

for i in range(len(ug.nodes)):
    update_dist(curr_node)
    next_node = get_min_unvisited(curr_node)

    curr_node = next_node
    visited[curr_node] = True


# OUTPUT
for item in dist.items():
    print(item)