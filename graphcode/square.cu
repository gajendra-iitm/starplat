#include <iostream>
#include <cuda.h>
#include <cmath>

// CUDA Kernel function to square a single number
__global__ void square(float *d_out, float *d_in) {
    *d_out = (*d_in) * (*d_in);
}

// CUDA Kernel function to calculate the square root
__global__ void squareRoot(float *d_out, float *d_in) {
    *d_out = sqrtf(*d_in);
}

int main() {
    // Number to be squared
    float h_in = 3.0f; // Change this value to square a different number
    float h_squared, h_sqrt;

    // Device pointers
    float *d_in, *d_squared, *d_sqrt;

    // Allocate memory on the device
    cudaMalloc((void**)&d_in, sizeof(float));
    cudaMalloc((void**)&d_squared, sizeof(float));
    cudaMalloc((void**)&d_sqrt, sizeof(float));

    // Copy input data from host to device
    cudaMemcpy(d_in, &h_in, sizeof(float), cudaMemcpyHostToDevice);

    // Launch kernel to square the number
    square<<<1, 1>>>(d_squared, d_in);
    cudaDeviceSynchronize(); // Ensure the square operation is complete

    // Launch kernel to calculate the square root of the squared number
    squareRoot<<<1, 1>>>(d_sqrt, d_squared);
    cudaDeviceSynchronize(); // Ensure the square root operation is complete

    // Copy results back from device to host
    cudaMemcpy(&h_squared, d_squared, sizeof(float), cudaMemcpyDeviceToHost);
    cudaMemcpy(&h_sqrt, d_sqrt, sizeof(float), cudaMemcpyDeviceToHost);

    // Print the results
    std::cout << "The square of " << h_in << " is " << h_squared << std::endl;
    std::cout << "The square root of " << h_squared << " is " << h_sqrt << std::endl;

    // Free device memory
    cudaFree(d_in);
    cudaFree(d_squared);
    cudaFree(d_sqrt);

    return 0;
}

