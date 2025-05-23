#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <cuda_runtime.h>

using namespace std;

int extractNodeCount(const std::string& file_path) {
    ifstream file(file_path);
    string line;
    getline(file, line);
    istringstream iss(line);
    string word;
    int count = -1;
    while (iss >> word) {
        try {
            count = std::stoi(word);  // This will only work on the number
        } catch (...) {
            // ignore words that aren't numbers
        }
    }
    return count;
}

void extractMapping(std::ifstream& file, std::vector<int>& host_map, const std::string& key_string) {
    file.clear();                 // Clear any EOF flags
    file.seekg(0);                // Go back to beginning of file
    std::string line;
    while (std::getline(file, line))
    {
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        if (line == key_string)
        {
            getline(file,line);
            // Find the part inside the curly braces
            size_t start = line.find('{');
            size_t end = line.find('}');
            if (start == std::string::npos || end == std::string::npos)
            {
                std::cerr << "\n Invalid format in line: " << line << std::endl;
                return;
            }

            std::string content = line.substr(start + 1, end - start - 1);
            std::istringstream iss(content);
            std::string token;

            while (std::getline(iss, token, ','))
            {
                size_t colon_pos = token.find(':');
                if (colon_pos != std::string::npos)
                {
                    int node = std::stoi(token.substr(0, colon_pos));
                    int community = std::stoi(token.substr(colon_pos + 1));
                    host_map[node] = community;
                }
            }
            break; // Stop after finding the desired section
        }
    }
}

__global__ void comparePairsKernel(const int* map1, const int* map2, int num_nodes, unsigned long long* d_similar, unsigned long long* d_similar1, unsigned long long* d_similar2) {
    unsigned long long idx = blockIdx.x * blockDim.x + threadIdx.x;

    unsigned long long total_pairs = (unsigned long long)num_nodes * (num_nodes - 1) / 2;
    if (idx >= total_pairs) return;
	
	if(idx == 0)
	{
		printf("%llu \n",total_pairs);
	}

    // Map linear idx to (i, j)
    int i = 0;
    while (idx >= num_nodes - i - 1) {
        idx -= (num_nodes - i - 1);
        i++;
    }
    int j = i + 1 + idx;
    int p = map1[i];
    int q = map1[j];
    int m = map2[i];
    int n = map2[j];

    if ((p == q && m == n) || (p != q && m != n)) {
        atomicAdd(d_similar, 1ULL);
	if(p==q && m==n)
	{
	    atomicAdd(d_similar1, 1ULL);	
	}
	else
	{
	    atomicAdd(d_similar2, 1ULL);
	}
    }
}

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Usage: ./compare_communities cpu_output.txt gpu_output.txt\n";
        return 1;
    }

    ifstream cpuFile(argv[1]);
    ifstream gpuFile(argv[2]);
    string line;

    int num_nodes = extractNodeCount(argv[2]);
    if (num_nodes <= 0) {
        std::cerr << "Invalid number of nodes found.\n";
        return 1;
    }

    // Allocate device memory for the 4 community mapping vectors
    int *d_cpu_init_map, *d_cpu_final_map, *d_gpu_init_map, *d_gpu_final_map;

    cudaMalloc(&d_cpu_init_map, sizeof(int) * num_nodes);
    cudaMalloc(&d_cpu_final_map, sizeof(int) * num_nodes);
    cudaMalloc(&d_gpu_init_map, sizeof(int) * num_nodes);
    cudaMalloc(&d_gpu_final_map, sizeof(int) * num_nodes);

    vector<int> h_cpu_init_map(num_nodes, -1);
    vector<int> h_cpu_final_map(num_nodes, -1);
    vector<int> h_gpu_init_map(num_nodes, -1);
    vector<int> h_gpu_final_map(num_nodes, -1);

    extractMapping(cpuFile, h_cpu_init_map, "Initial Community Mapping:");
    extractMapping(cpuFile, h_cpu_final_map, "Final Community Mapping:");
    extractMapping(gpuFile, h_gpu_init_map, "Initial Community Mapping:");
    extractMapping(gpuFile, h_gpu_final_map, "Final Community Mapping:");

	int min_init = *std::min_element(h_cpu_init_map.begin(), h_cpu_init_map.end());
	int max_init = *std::max_element(h_cpu_init_map.begin(), h_cpu_init_map.end());

	int min_final = *std::min_element(h_cpu_final_map.begin(), h_cpu_final_map.end());
	int max_final = *std::max_element(h_cpu_final_map.begin(), h_cpu_final_map.end());

	int min_gpu_init = *std::min_element(h_gpu_init_map.begin(), h_gpu_init_map.end());
	int max_gpu_init = *std::max_element(h_gpu_init_map.begin(), h_gpu_init_map.end());

	int min_gpu_final = *std::min_element(h_gpu_final_map.begin(), h_gpu_final_map.end());
	int max_gpu_final = *std::max_element(h_gpu_final_map.begin(), h_gpu_final_map.end());

	std::cout << "Initial CPU Mapping: Min = " << min_init << ", Max = " << max_init << std::endl;
	std::cout << "Final CPU Mapping: Min = " << min_final << ", Max = " << max_final << std::endl;
	std::cout << "GPU Initial: Min = " << min_gpu_init << ", Max = " << max_gpu_init << std::endl;
	std::cout << "GPU Final:   Min = " << min_gpu_final << ", Max = " << max_gpu_final << std::endl;
	cout<<"\n\n\n";

    cudaMemcpy(d_cpu_init_map, h_cpu_init_map.data(), sizeof(int) * num_nodes, cudaMemcpyHostToDevice);
    cudaMemcpy(d_cpu_final_map, h_cpu_final_map.data(), sizeof(int) * num_nodes, cudaMemcpyHostToDevice);
    cudaMemcpy(d_gpu_init_map, h_gpu_init_map.data(), sizeof(int) * num_nodes, cudaMemcpyHostToDevice);
    cudaMemcpy(d_gpu_final_map, h_gpu_final_map.data(), sizeof(int) * num_nodes, cudaMemcpyHostToDevice);

    unsigned long long *d_similar;
    cudaMalloc(&d_similar, sizeof(long long));
    cudaMemset(d_similar, 0, sizeof(long long));

    unsigned long long *d_similar1;
    cudaMalloc(&d_similar1, sizeof(long long));
    cudaMemset(d_similar1, 0, sizeof(long long));

    unsigned long long *d_similar2;
    cudaMalloc(&d_similar2, sizeof(long long));
    cudaMemset(d_similar2, 0, sizeof(long long));

    // unsigned long long total_pairs = (unsigned long long)num_nodes * (num_nodes - 1) / 2;
    int x = min(num_nodes,100000);
    unsigned long long total_pairs = (unsigned long long)x * (x - 1) / 2;
    int threads_per_block = 1024;
    int blocks = (total_pairs + threads_per_block - 1) / threads_per_block;


    // comparePairsKernel<<<blocks, threads_per_block>>>(d_cpu_init_map, d_gpu_init_map, num_nodes, d_similar);

//int x = min(num_nodes,1000);
comparePairsKernel<<<blocks, threads_per_block>>>(d_cpu_init_map, d_gpu_init_map, x, d_similar, d_similar1, d_similar2); 

    unsigned long long h_similar = 0, h_different = 0;
    cudaMemcpy(&h_similar, d_similar, sizeof(long long), cudaMemcpyDeviceToHost);
    h_different = total_pairs - h_similar;

    unsigned long long h_similar1 = 0, h_similar2 = 0;
    cudaMemcpy(&h_similar1, d_similar1, sizeof(long long), cudaMemcpyDeviceToHost);
    cudaMemcpy(&h_similar2, d_similar2, sizeof(long long), cudaMemcpyDeviceToHost);

    float score = (total_pairs > 0) ? (float)h_similar / total_pairs : 1.0;

    cout<<"For initial mapping:\n";
    std::cout << "Similarity: " << h_similar << ", Different: " << h_different
              << ", Total: " << total_pairs << ", Score: " << score << "\n";

    cout<<"Similarity 1: "<<h_similar1<<", Similarity 2: "<<h_similar2<<endl;

    cudaMemset(d_similar, 0, sizeof(long long));
    cudaMemset(d_similar1, 0, sizeof(long long));
    cudaMemset(d_similar2, 0, sizeof(long long));
    
    // comparePairsKernel<<<blocks, threads_per_block>>>(d_cpu_final_map, d_gpu_final_map, num_nodes, d_similar);
    
comparePairsKernel<<<blocks, threads_per_block>>>(d_cpu_final_map, d_gpu_final_map, x, d_similar, d_similar1, d_similar2);
    
    cudaMemcpy(&h_similar, d_similar, sizeof(long long), cudaMemcpyDeviceToHost);
    h_different = total_pairs - h_similar;

    cudaMemcpy(&h_similar1, d_similar1, sizeof(long long), cudaMemcpyDeviceToHost);
    cudaMemcpy(&h_similar2, d_similar2, sizeof(long long), cudaMemcpyDeviceToHost);

    score = (total_pairs > 0) ? (float)h_similar / total_pairs : 1.0;

    cout<<"For final mapping:\n";
    std::cout << "Similarity: " << h_similar << ", Different: " << h_different
              << ", Total: " << total_pairs << ", Score: " << score << "\n";

    cout<<"Similarity 1: "<<h_similar1<<", Similarity 2: "<<h_similar2<<endl;

    cudaFree(d_similar);
    cudaFree(d_cpu_init_map);
    cudaFree(d_cpu_final_map);
    cudaFree(d_gpu_init_map);
    cudaFree(d_gpu_final_map);

    return 0;
}
