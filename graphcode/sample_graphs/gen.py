import sys
sys.path.append('/opt/homebrew/opt/graph-tool/lib/python3.12/site-packages/')
from graph_tool.all import *
from random import randint

# Get N, deg1, deg2 as args
N = int(sys.argv[1])
degS = int(sys.argv[2])
degE = int(sys.argv[3])

print(N, degS, degE)

# Generate a random graph
g = random_graph(N, lambda: randint(degS, degE), directed=False)

# Save as src, dest, wt to a file
with open('graph.txt', 'w') as f:
  for edge in g.get_edges([g.edge_index]):
    f.write(f"{edge[0]} {edge[1]} {edge[2]}\n")

# Draw the graph out in a new window with node labels
pos = sfdp_layout(g)  # Use a layout for better visualization
if N < 20:
    graph_draw(g, pos=pos, output_size=(200, 200),
           vertex_text=g.vertex_index,  # Add node labels
           vertex_font_size=18,  # Adjust font size for readability
           output="graph.png")
else:
    graph_draw(g, pos=pos, output_size=(200, 200),
               output="graph.png")

# # Compute page rank and save to a file
pr = pagerank(g, damping=0.85, epsilon=0.01, max_iter=100)
with open('pagerank.txt', 'w') as f:
  for i in range(N):
    f.write(f"{i} {pr[i]}\n")

# Compute betweenness centrality and save to a file
bc = betweenness(g, [0], norm = False)
vertex_betweenness = bc[0].get_array()
with open('betweenness.txt', 'w') as f:
  for i in range(N):
      #Print as float
        f.write(f"Node {i}: {vertex_betweenness[i]}\n")
#
# # Compute