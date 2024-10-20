#include <iostream>
#include <cuda_runtime.h>

// Define the Layer struct
struct Layer {
    int num_features;
    float *weights;
    float *aggregatedFeatures;
    float *preActivatedFeatures;
};

// Kernel function
__global__ void NeuralNetworkOperation(Layer *layer, int numNodes, int layerNumber)
{
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    if (idx < numNodes)
    {
        for (int i = 0; i < layer[layerNumber + 1].num_features; i++) // For each feature in the next layer
        {
            float sum = 0.0f;
            for (int j = 0; j < layer[layerNumber].num_features; j++) // For each feature in the current layer
            {
                sum += layer[layerNumber].weights[i * layer[layerNumber].num_features + j] *
                       layer[layerNumber].aggregatedFeatures[idx * layer[layerNumber].num_features + j];
                
                // Debugging output
                if (idx == 0 && j < 3) // Print for the first node and a few weights/features
                {
                    printf("Node %d: weight[%d] = %f, feature[%d] = %f, partial sum = %f\n",
                           idx, i * layer[layerNumber].num_features + j, 
                           layer[layerNumber].weights[i * layer[layerNumber].num_features + j], 
                           idx * layer[layerNumber].num_features + j, 
                           layer[layerNumber].aggregatedFeatures[idx * layer[layerNumber].num_features + j], 
                           sum);
                }
            }
            layer[layerNumber].preActivatedFeatures[idx * layer[layerNumber + 1].num_features + i] = sum;
        }
    }
}

int main()
{
    // Define the sizes and number of layers (example values)
    const int numLayers = 2; // Number of layers
    const int numNodes = 4; // Number of nodes in the current layer
    const int numFeatures = 3; // Number of features in each layer
    const int numFeaturesNextLayer = 2; // Number of features in the next layer

    // Allocate memory for layers
    Layer *d_layer;
    cudaMalloc(&d_layer, sizeof(Layer) * numLayers);

    // Initialize Layer structs and allocate device memory for weights and features
    Layer h_layer[numLayers];
    // Example Initialization (make sure to use realistic values)
float h_weights[] = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f}; // Example weights
float h_features[] = {1.0f, 2.0f, 3.0f, 4.0f}; // Example features

cudaMemcpy(d_layer[0].weights, h_weights, sizeof(h_weights), cudaMemcpyHostToDevice);
cudaMemcpy(d_layer[0].aggregatedFeatures, h_features, sizeof(h_features), cudaMemcpyHostToDevice);

    for (int i = 0; i < numLayers; ++i) {
        h_layer[i].num_features = (i == numLayers - 1) ? numFeaturesNextLayer : numFeatures;
        cudaMalloc(&h_layer[i].weights, h_layer[i].num_features * (i == numLayers - 1 ? numFeatures : numFeatures) * sizeof(float));
        cudaMalloc(&h_layer[i].aggregatedFeatures, numNodes * h_layer[i].num_features * sizeof(float));
        cudaMalloc(&h_layer[i].preActivatedFeatures, numNodes * (i == numLayers - 1 ? numFeaturesNextLayer : numFeatures) * sizeof(float));
        cudaMemcpy(&d_layer[i], &h_layer[i], sizeof(Layer), cudaMemcpyHostToDevice);
    }

    // Launch the kernel
    int blockSize = 4;
    int gridSize = (numNodes + blockSize - 1) / blockSize;
    NeuralNetworkOperation<<<gridSize, blockSize>>>(d_layer, numNodes, 0);

    // Check for CUDA errors
    cudaDeviceSynchronize();
    cudaError_t err = cudaGetLastError();
    if (err != cudaSuccess)
    {
        std::cerr << "CUDA error: " << cudaGetErrorString(err) << std::endl;
    }

    // Copy results back and print
    float *h_preActivatedFeatures = new float[numNodes * numFeaturesNextLayer];
    cudaMemcpy(h_preActivatedFeatures, h_layer[0].preActivatedFeatures, numNodes * numFeaturesNextLayer * sizeof(float), cudaMemcpyDeviceToHost);

    std::cout << "Pre-activated features after kernel execution:\n";
    for (int i = 0; i < numNodes; i++)
    {
        for (int j = 0; j < numFeaturesNextLayer; j++)
        {
            std::cout << h_preActivatedFeatures[i * numFeaturesNextLayer + j] << " ";
        }
        std::cout << "\n";
    }

    delete[] h_preActivatedFeatures;

    // Free device memory
    for (int i = 0; i < numLayers; ++i) {
        cudaFree(h_layer[i].weights);
        cudaFree(h_layer[i].aggregatedFeatures);
        cudaFree(h_layer[i].preActivatedFeatures);
    }
    cudaFree(d_layer);

    return 0;
}
