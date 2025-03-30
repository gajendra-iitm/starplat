#include <thrust/device_vector.h>
#include <thrust/host_vector.h>
#include <cuda_runtime.h>
#include <iostream>

// CUDA Kernel
__global__ void kernel(float* d_data, int N) {
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    if (idx < N) {
        d_data[idx] *= idx; // Example computation
    }
}

int main() {
    int N = 10;

    // Host vector
    thrust::host_vector<float> h_vec(N, 1.0f);

    // Device vector
    thrust::device_vector<float> d_vec = h_vec;

    // Get raw pointer to device data
    float* d_ptr = thrust::raw_pointer_cast(d_vec.data());

    // Launch Kernel
    kernel<<<1, N>>>(d_ptr, N);
    cudaDeviceSynchronize();
    cudaError_t err = cudaGetLastError();
if (err != cudaSuccess) {
    std::cerr << "CUDA Error: " << cudaGetErrorString(err) << std::endl;
}


    thrust::host_vector<float> h(N,2.9f);
    // Copy data back to host
    thrust::copy(d_vec.begin(), d_vec.end(), h.begin());

    // Print results
    for (int i = 0; i < N; i++) {
        std::cout << h[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}

