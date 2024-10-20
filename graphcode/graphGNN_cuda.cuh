#include <cuda.h>
#include"graph.hpp"
#include<vector>
#include <curand_kernel.h>


class __device__ Layer {
public:
    int32_t num_features;
    double *weights; 
    double *bias;
    double epsilon;
    double grad_epsilon;
    double *aggregatedFeatures;
    double *preActivatedFeatures;
    double *postActivatedFeatures;
    double *grad_pre_act_output;
    double *grad_weights;
    double *grad_bias;
    double *m_weights;
    double *m_biases;
    double m_epsilon;
    double v_epsilon;
    double *v_weights;
    double *v_biases;
};


__device__ void xaviersInit_cuda(double *weights, int numInputs, int numOutputs) {
    double epsilon = sqrt(6) / sqrt(numInputs + numOutputs);
    curandState state;
    curand_init(0, 0, 0, &state);
    for (int i = 0; i < numInputs; i++) {
        for (int j = 0; j < numOutputs; j++) {
            weights[i * numOutputs + j] = curand_uniform(&state) * 2 * epsilon - epsilon;
        }
    }
}


__device__ void heInit_cuda(double *weights, int numInputs, int numOutputs) {
    double epsilon = sqrt(2) / sqrt(numInputs);
    curandState state;
    curand_init(0, 0, 0, &state);
    for (int i = 0; i < numInputs; i++) {
        for (int j = 0; j < numOutputs; j++) {
            weights[i * numOutputs + j] = curand_uniform(&state) * 2 * epsilon - epsilon;
        }
    }
}

__device__ void uniformDistributionInit_cuda(double *weights, int numInputs, int numOutputs) {
    curandState state;
    curand_init(0, 0, 0, &state);
    for (int i = 0; i < numInputs; i++) {
        for (int j = 0; j < numOutputs; j++) {
            weights[i * numOutputs + j] = curand_uniform(&state) * 2 - 1;
        }
    }
}


__global__ void initializeLayers_kernel(layers *d_layers, int *neuronsPerLayer, int numLayers, int numNodes, char *initType) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    
    if (i < numLayers) {
        int numFeatures = neuronsPerLayer[i];
        d_layers[i].num_features = numFeatures;

        
        cudaMalloc(&d_layers[i].weights, neuronsPerLayer[i - 1] * sizeof(double*));         // Initialize weights and biases
        cudaMalloc(&d_layers[i].bias, numFeatures * sizeof(double));

        
        cudaMalloc(&d_layers[i].preActivatedFeatures, numNodes * sizeof(double*));     // Initialize pre-activated, post-activated, and aggregated features
        cudaMalloc(&d_layers[i].postActivatedFeatures, numNodes * sizeof(double*));
        cudaMalloc(&d_layers[i].aggregatedFeatures, numNodes * sizeof(double*));

        for (int j = 0; j < numNodes; j++) {
            cudaMalloc(reinterpret_cast<void**>(&d_layers[i].preActivatedFeatures[j]), 3 * sizeof(double));
            cudaMalloc(reinterpret_cast<void**>(&d_layers[i].postActivatedFeatures[j]), 3 * sizeof(double));
            cudaMalloc(reinterpret_cast<void**>(&d_layers[i].aggregatedFeatures[j]), 3 * sizeof(double));

        }

        
        if (strcmp(initType, "xaviers") == 0) {             // Initialize with Xavier, He, or other methods
            xaviersInit_cuda(d_layers[i].weights, neuronsPerLayer[i - 1], numFeatures);
        } else if (strcmp(initType, "he") == 0) {
            heInit_cuda(d_layers[i].weights, neuronsPerLayer[i - 1], numFeatures);
        } else if (strcmp(initType, "uniform") == 0) {
            uniformDistributionInit_cuda(d_layers[i].weights, neuronsPerLayer[i - 1], numFeatures);
        }
    }
}


// Activation functions
__device__ double tanh(double x) {
    return (exp(x) - exp(-x)) / (exp(x) + exp(-x));
}

__device__ double relu(double x) {
    return x > 0 ? x : 0;
}

__device__ void softmax_cuda(double *x, int numFeatures) {
    double max = x[0];
    for (int i = 1; i < numFeatures; i++) {
        if (x[i] > max) {
            max = x[i];
        }
    }

    double sum = 0.0;
    for (int i = 0; i < numFeatures; i++) {
        x[i] = exp(x[i] - max);
        sum += x[i];
    }

    for (int i = 0; i < numFeatures; i++) {
        x[i] /= sum;
    }
}


// Derivative of activation functions
__device__ double derivative_tanh(double x) {
    return 1 - x * x;
}

__device__ double derivative_relu(double x) {
    return x > 0 ? 1 : 0;
}

__device__ void derivative_softmax(double *x, int numFeatures) {
    for (int i = 0; i < numFeatures; i++) {
        for (int j = 0; j < numFeatures; j++) {
            if (i == j) {
                x[i] *= (1 - x[j]);
            } else {
                x[i] *= -x[j];
            }
        }
    }
}


__global__ void aggregationKernel(double *d_postActivatedFeatures, double *d_aggregatedFeatures, 
                                        int *d_neighborOffsets, int *d_destinations, 
                                        float *d_edgeWeights, int numNodes, int numFeatures, int layerNumber) {
    
    int node = blockIdx.x;  // Each block processes one node
    int featureStart = threadIdx.x;  // Each thread processes one feature

    int aggregatedFeatureIdx = node * numFeatures + featureStart;

    if (featureStart < numFeatures) {
        d_aggregatedFeatures[aggregatedFeatureIdx] = 0.0;
    }

    // Getting the neighbors for the current node
    int neighborStart = d_neighborOffsets[node];
    int neighborEnd = d_neighborOffsets[node + 1];

    // Accumulate features from neighbors
    for (int neighborIdx = neighborStart; neighborIdx < neighborEnd; neighborIdx++) {
        int neighbor = d_destinations[neighborIdx];
        float edgeWeight = d_edgeWeights[neighborIdx];

        if (featureStart < numFeatures) {
            int neighborFeatureIdx = neighbor * numFeatures + featureStart;
            d_aggregatedFeatures[aggregatedFeatureIdx] += d_postActivatedFeatures[neighborFeatureIdx] * edgeWeight;
        }
    }
}



//forward pass
__global__ void forwardPass_kernel(layer *d_layers, int layerNumber, int numNodes, int aggregationType) {
    int node = blockIdx.x * blockDim.x + threadIdx.x;
    aggregationKernel<<<numNodes, d_layers[layerNumber].num_features>>>(d_layers[layerNumber].d_postActivatedFeatures, d_layers[layerNumber].d_aggregatedFeatures, layerNumber, numNodes, node, aggregationType);
    
    if (node < numNodes) {

        for (int i = 0; i < d_layers[layerNumber].num_features; i++) {
            double sum = 0.0;

            for (int j = 0; j < d_layers[layerNumber - 1].num_features; j++) {
                sum += d_layers[layerNumber - 1].postActivatedFeatures[node][j] * d_layers[layerNumber].weights[j][i];
            }

            d_layers[layerNumber].preActivatedFeatures[node][i] = sum + d_layers[layerNumber].bias[i];

            d_layers[layerNumber].postActivatedFeatures[node][i] = tanh(d_layers[layerNumber].preActivatedFeatures[node][i]);
        }

        if (layerNumber == 6 - 1) {
            softmax_cuda(d_layers[layerNumber].postActivatedFeatures[node], d_layers[layerNumber].num_features);
        }
    }
}

#define TILE_WIDTH 32  // For tiled matrix multiplication

__global__ void backPropagation_cuda(
    double *d_preActivatedFeatures, double *d_postActivatedFeatures,
    double *d_grad_pre_act_output, double *d_grad_weights, double *d_weights,
    double *d_grad_bias, double *d_bias, double *d_grad_epsilon,
    int numNodes, int numFeatures, int nextLayerFeatures,
    int aggregationType, double epsilon, double learning_rate)
{
    // Weight gradient computation with matrix multiplication and tiling
    __shared__ double shared_A[TILE_WIDTH][TILE_WIDTH];
    __shared__ double shared_B[TILE_WIDTH][TILE_WIDTH];

    int row = blockIdx.y * TILE_WIDTH + threadIdx.y;
    int col = blockIdx.x * TILE_WIDTH + threadIdx.x;

    double gradWeightValue = 0.0;

    // Perform tiled matrix multiplication for gradient weights
    for (int t = 0; t < (numFeatures - 1) / TILE_WIDTH + 1; ++t) {
        // Load data into shared memory
        if (row < numNodes && t * TILE_WIDTH + threadIdx.x < numFeatures) {
            shared_A[threadIdx.y][threadIdx.x] = d_aggregatedFeatures[row * numFeatures + t * TILE_WIDTH + threadIdx.x];
        } else {
            shared_A[threadIdx.y][threadIdx.x] = 0.0;
        }

        if (t * TILE_WIDTH + threadIdx.y < numFeatures && col < nextLayerFeatures) {
            shared_B[threadIdx.y][threadIdx.x] = d_grad_pre_act_output[(t * TILE_WIDTH + threadIdx.y) * nextLayerFeatures + col];
        } else {
            shared_B[threadIdx.y][threadIdx.x] = 0.0;
        }

        __syncthreads();

        // Multiply and accumulate
        for (int i = 0; i < TILE_WIDTH; ++i) {
            gradWeightValue += shared_A[threadIdx.y][i] * shared_B[i][threadIdx.x];
        }

        __syncthreads();
    }

    // Store result
    if (row < numFeatures && col < nextLayerFeatures) {
        d_grad_weights[row * nextLayerFeatures + col] = gradWeightValue;
    }

    // Compute bias gradients
    if (threadIdx.x == 0) {
        for (int n = 0; n < numNodes; ++n) {
            atomicAdd(&d_grad_bias[blockIdx.y], d_grad_pre_act_output[n * nextLayerFeatures + blockIdx.y]);
        }
    }

    // Apply activation derivative and compute epsilon gradient (if using GIN)
    if (row < numNodes && col < numFeatures) {
        if (aggregationType == 2) {  // GIN aggregation
            d_grad_epsilon[row] += d_grad_pre_act_output[row * numFeatures + col] * d_postActivatedFeatures[row * numFeatures + col];
        }

        // Apply derivative of activation function (ReLU)
        d_grad_pre_act_output[row * numFeatures + col] *= (d_preActivatedFeatures[row * numFeatures + col] > 0.0 ? 1.0 : 0.0);
    }
}

__global__ void adamOptimizer(double *d_weights, double *d_grad_weights, double *d_bias, double *d_grad_bias,
                               double learning_rate, int numFeatures, int nextLayerFeatures) {
   //TODO: Implement optimizer step
}








void initializeLayers_cuda(GNN &gnn, std::vector<int32_t> neuronsPerHiddenLayer, char *initType) {

    int *d_neuronsPerLayer;
    int numLayers = neuronsPerHiddenLayer.size() + 2;
    int *neuronsPerLayer = new int[numLayers];

    neuronsPerLayer[0] = gnn.numFeatures();
    for (int i = 0; i < neuronsPerHiddenLayer.size(); i++) {
        neuronsPerLayer[i + 1] = neuronsPerHiddenLayer[i];
    }
    neuronsPerLayer[numLayers - 1] = gnn.numClasses();

    cudaMalloc(&d_neuronsPerLayer, numLayers * sizeof(int));
    cudaMemcpy(d_neuronsPerLayer, neuronsPerLayer, numLayers * sizeof(int), cudaMemcpyHostToDevice);

    globalCudaVar *d_layers;
    cudaMalloc(&d_layers, numLayers * sizeof(globalCudaVar));


    int threads = 128;
    int blocks = (numLayers + threads - 1) / threads;
    initializeLayers_kernel<<<blocks, threads>>>(d_layers, d_neuronsPerLayer, numLayers, gnn.getGraph().num_nodes(), initType);

    cudaDeviceSynchronize();

}

void forwardProp_cuda(){
    //todo: implement forward pass
}

void backPropogation_cuda(){
    //todo: implement backpropagation
}
void adamOptimizer_cuda(){
    //todo: implement optimizer
}
