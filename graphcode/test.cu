#include <iostream>
#include <cuda_runtime.h>

class MyClass {
public:
    int32_t numEyes;        // Integer data member
    double* numMistakes;    // Pointer to a dynamically allocated array

    // Constructor to initialize numEyes and prepare memory for numMistakes
    __device__ MyClass() : numEyes(0), numMistakes(nullptr) {}

    // Method to initialize the object on the device
    __device__ void initialize(int32_t eyes, int32_t array_size) {
        numEyes = eyes;
        numMistakes = new double[array_size];  // Allocate memory on the device
        for (int i = 0; i < array_size; i++) {
            numMistakes[i] = static_cast<double>(i) * 1.1;  // Initialize the array
        }
    }

    // Method to free dynamically allocated memory
    __device__ void freeArray() {
        delete[] numMistakes;  // Free the dynamically allocated array
    }

    // Print method for debugging (prints the data)
    __device__ void printData(int32_t array_size) {
        printf("numEyes: %d\n", numEyes);
        for (int i = 0; i < array_size; i++) {
            printf("numMistakes[%d] = %f\n", i, numMistakes[i]);
        }
    }
};

// Kernel to initialize an array of MyClass objects and print their values
__global__ void initializeAndProcess(MyClass* myClassArray, int32_t array_size, int32_t num_classes) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Initialize each MyClass object in the array
        myClassArray[idx].initialize(idx, array_size);  // Pass a unique numEyes and array size

        // Print the values for debugging (executed on the GPU)
        for(int i = 0;i<array_size;i++){
            printf("numMistakes[%d] = %f,  numEyes  = %d\n", i, myClassArray[idx].numMistakes[i],myClassArray[idx] .numEyes);
        }
    }


// Kernel to clean up the dynamically allocated arrays in MyClass objects
__global__ void freeMemory(MyClass* myClassArray, int32_t num_classes) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < num_classes) {
        myClassArray[idx].freeArray();  // Free each object's array
    }
}

int main() {
    const int32_t num_classes = 3;    // Number of MyClass objects
    const int32_t array_size = 5;     // Size of each numMistakes array

    // Allocate memory for an array of MyClass objects on the device
    MyClass* d_myClassArray;
    cudaMalloc(&d_myClassArray, num_classes * sizeof(MyClass));

    // Launch kernel to initialize the objects and their arrays
    initializeAndProcess<<<1, num_classes>>>(d_myClassArray, array_size, num_classes);
    cudaDeviceSynchronize();

    // Launch kernel to free dynamically allocated memory in each object
    freeMemory<<<1, num_classes>>>(d_myClassArray, num_classes);
    cudaDeviceSynchronize();

    // Free the device memory allocated for the array of objects


    return 0;
}
