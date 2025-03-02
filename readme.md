# StarPlat
---
### Installation and Usage Instruction for linux/WSL

##### Make sure you have build-essential (latest), bison 3.8.2, gcc 11, g++ 11, flex 2.6.4, libomp-dev
 Use the following command in the debian terminal to install all the required tools
 ``` sudo apt install build-essential gcc g++ bison flex libomp-dev ```

### Making the StarPlat src

Get to the src folder of the repository and run the following command in the terminal
``` make ```

### Compiling the DSL Codes
To Compile the dsl codes to generate OMP specific code use the following command in the terminal
```
./StarPlat [-s|-d] -f <dsl.sp> -b [cuda|omp|mpi|acc|sycl]

#Example
./StarPlat -s -f ../graphcode/staticDSLCodes/triangle_counting_dsl -b omp

-s for static and -d for dynamic
-b select the type of backend
-f the dsl file to input
```
>Once the omp code is generated it would be reflected in generated_omp folder of the graphcode directory in the repository

### Running the OMP code

In the graphcode folder there is a main.cpp file, edit out the dsl file name in the header and modify path for the loading different graphs and then compile and run using g++.
```  
export OMP_NUM_THREADS=16
g++ main.cpp -o main -fopenmp 
./main
```
## Publication
[StarPlat: A versatile DSL for graph analytics](https://www.sciencedirect.com/science/article/pii/S074373152400131X); Nibedita Behera, Ashwina Kumar, Ebenezer Rajadurai T, Sai Nitish, Rajesh Pandian M, Rupesh Nasre; IJPP 2024


## Acknowledgments
```
This project is funded by India's National Supercomputing Mission, whose timely funding and regular reviews are greatly appreciated.
Part of the work is supported by Qualcomm Innovation Fellowship 2023. 
```
## Licensing
The StarPlat codebase is made available for academic and non-commercial use.
StarPlat is patented. Commercial use of the code needs licensing. The interested may contact rupesh@cse.iitm.ac.in.



## Betweeness centrality
What is betweeness centrality: 
[Science Direct](https://www.sciencedirect.com/topics/computer-science/betweenness-centrality)
[Wikipedia](https://en.wikipedia.org/wiki/Betweenness_centrality)
[IITM BS Data Science video on Betweeness Centrality](https://youtu.be/Cziv7Xfl8Zs?t=211)

Computing Betweeness centrality in [Python networkX library](https://networkx.org/documentation/stable/reference/algorithms/generated/networkx.algorithms.centrality.betweenness_centrality.html)

what is source nodes input parameter in betweeness centrality function:
as we know the bc algorithm computes bc values for all pairs (source node, target node) of nodes in the graph; but if we need to computer bc only taking certain nodes as source nodes while taking all the other nodes as taget nodes. Then we need to supply our set of source nodes as input parameter to bc function. i.e; i need to compute bc only for 2 source nodes 0, 1: then the .txt file must have these source nodes in this format:
```0
1```


--> way to do it in generated static bc cuda code bc_dslV2.cu using bc_dslV2mainCuda.cu:

1) make sure to first generate the cuda backend code:
   by cd into 'src'
   ```cd starplat/src```
   command to run on your terminal to *generate* "betweeness centrality (for static graphs) cuda backend code" using bc_dslV2 as input dsl:
   ```./StarPlat -s -f ../graphcode/staticDSLCodes/bc_dslV2 -b cuda```
   note: for executing on Google collab cell: remember to add ! before the above command.

2) generating the bc_dslV2 cuda backend code by using the above command will generate two files 'bc_dslV2.cu' and 'bc_dslV2.h' 
   in the directory ../graphcode/generated_cuda which is required for this code to run. As we have the main caller function for that code written here in this file. *Compile* using nvcc:
   ```nvcc bc_dslV2mainCuda.cu -o bc_dslV2mainCuda -arch=sm_70 -std=c++14 -rdc=true```

3) *run*: 
   ```./bc_dslV2mainCuda.out path/to/your/graph.txt path/to/your/srcNodes.txt```

--> the same can be done in python *networkX* library:

```betweenness_centrality = networkx.betweenness_centrality_subset(
    G,                     # The graph
    sources=sourceSet,     # Source nodes
    targets=G.nodes(),     # Target nodes (all nodes in the graph)
    normalized=True        # Normalize the centrality values
)```