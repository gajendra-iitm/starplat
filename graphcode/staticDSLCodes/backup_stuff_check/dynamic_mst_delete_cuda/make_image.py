import networkx as nx
import matplotlib.pyplot as plt

# Create a weighted undirected graph with 6 nodes and 15 edges
G = nx.Graph()

# Add edges with weights
edges = [
    (0, 1, 4), (0, 2, 6), (0, 3, 7), (1, 2, 3), (1, 4, 8), (1, 5, 2),
    (2, 3, 5), (2, 4, 9), (3, 4, 2), (3, 5, 6), (4, 5, 4), (4, 6, 3),
    (5, 6, 8), (2, 6, 7), (1, 3, 6)
]

G.add_weighted_edges_from(edges)

# Find the MST of the graph using Kruskal's algorithm
mst = nx.minimum_spanning_tree(G)

# Define edge colors: green for MST edges, black for others
edge_colors = ['green' if (u, v) in mst.edges() or (v, u) in mst.edges() else 'black' for u, v, w in G.edges(data=True)]

for u, v, w in G.edges(data=True):
    if (u, v) in mst.edges() or (v, u) in mst.edges():
        print("u",u,"v",v)
    
# Draw the graph with weights on edges and clearer edges
plt.figure(figsize=(8, 6))
nx.draw(G, pos=nx.spring_layout(G), with_labels=True, node_size=500, node_color='white', font_weight='bold', font_size=12, edge_color=edge_colors, width=2)

# Add weights to the edges
edge_labels = {(u, v): G[u][v]['weight'] for u, v in G.edges()}
nx.draw_networkx_edge_labels(G, pos=nx.spring_layout(G), edge_labels=edge_labels, font_size=12)

plt.title('Weighted Graph with MST Edges Colored Green and Weights', fontsize=14)
plt.show()


