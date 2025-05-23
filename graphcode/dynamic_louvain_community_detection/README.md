# Dynamic Louvain Community Detection in CUDA and Python

## Project Description

This project implements **Louvain community detection** with support for **dynamic updates** to the graph (edge insertions and deletions). It includes:

- Two CUDA implementations:
  - Parallel community allocation
  - Sequential community allocation
- A Python implementation
- An output comparison tool

Each version processes an initial graph, applies dynamic updates, and outputs community mappings before and after the updates.

---

## Running the Code

### CUDA Implementations

#### Compile:

```bash
nvcc -arch=sm_70 -o parallel_exec communitydetection_parallel_allocation.cu
nvcc -arch=sm_70 -o sequential_exec communitydetection_sequential_allocation.cu
```

#### Run:

```bash
./parallel_exec graph.txt edges.txt > cuda_output.txt
# or
./sequential_exec graph.txt edges.txt > cuda_output.txt
```

### Python Implementation

```bash
python3 dynamic_louvain_cpu.py graph.txt edges.txt > cpu_output.txt
```

---

## Comparing Outputs

To compare the output from two implementations:

### Compile:

```bash
nvcc -o compare_exec compare_outputs.cu
```

### Run:

```bash
./compare_exec cpu_output.txt cuda_output.txt
```

This prints a similarity measure (e.g., percentage match) between the two output community mappings.

---

## Input Format

### `graph.txt`
```
p q w
q p w
```
- Each edge is listed **twice** (in both directions)
- `p`, `q`: vertices; `w`: edge weight

### `edges.txt`
```
a p q   # Add edge between p and q
d p q   # Delete edge between p and q
```
- Only **one direction** listed per edge
- All updates are assumed to be unit weight

---

## Output

Each code version produces:

- Community mapping for the original graph
- Community mapping after applying edge updates

These are printed to `stdout` or can be redirected to a file.
