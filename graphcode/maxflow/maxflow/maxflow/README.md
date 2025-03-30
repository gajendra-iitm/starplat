# Dynamic MST Calculation with Deletion of Edges in CUDA

## Project Description
This project implements dynamic calculation of MST of a graph stored in CSR format, when the updates are deletion of edges. 
The given code takes the original graph, updates file, and the updated graph as a file. It verifies the recalculated mst value by calculating the MST from the updated graph from the beginning. It also outputs the time taken on the GPU.




## Running the Code
1. Ensure the environment variables are set. If not, we can set it as follows.
```
export CUDA_HOME=/usr/local/cuda
export PATH=$CUDA_HOME/bin:$PATH
export LD_LIBRARY_PATH=$CUDA_HOME/lib64:$LD_LIBRARY_PATH

```
2. Compile the code:
    ```sh
    nvcc dynamic_MST_delete.cu -rdc=true -w -std=c++11 -o dynamic_MST_delete
    ```
3. Run the executable:
    ```sh
    ./dynamic_MST_delete <original_graph_file_path> <update_graph_file_after_deletion_path> <delete_edges_update_file_path>
    ```
Eg. 
     ```
    ./dynamic_MST_delete pokecudwt.txt updatespokecudwt.txt staticpokecudwt.txt
    ```

