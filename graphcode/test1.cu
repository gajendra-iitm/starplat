#include<iostream>
#include<cuda_runtime.h>

class Layer{
	public:
		int32_t num_features;
		double *weights;
		double *bias;
		double epsilon = 0.01;
		double grad_epsilon;
		double *aggregatedFeatures;
		double *preActivatedFeatures;
		double *postActivatedFeatures;
		double *grad_pre_act_output;
		double *grad_weights;
		double *grad_bias;
		double *m_weights;
		double *m_biases;
		double m_epsilon = 0.0f;
		double v_epsilon = 0.0f;
		double *v_weights;
		double *v_biases;

		//Constructor to initialize layers
		__device__ Layer() : num_features(0), weights(nullptr), bias(nullptr), epsilon(0.01), grad_epsilon(0.0), aggregatedFeatures(nullptr), preActivatedFeatures(nullptr), postActivatedFeatures(nullptr),grad_pre_act_output(nullptr), grad_weights(nullptr), grad_bias(nullptr), m_weights(nullptr), m_biases(nullptr), m_epsilon(0.0f), v_epsilon(0.0f), v_weights(nullptr), v_biases(nullptr) {}


		__device__ void initializeLayers(int32_t layer_num, int32_t num_feat_current, int32_t num_feat_prev, int32_t num_nodes){
			weights = new double[num_feat_prev * num_feat_current];
			bias = new double[ num_feat_current];
			grad_weights = new double[num_feat_prev * num_feat_current];
			grad_bias = new double[num_feat_current];
			m_weights = new double[num_feat_prev * num_feat_current];
			m_biases = new double[num_feat_current];
			v_weights = new double[num_feat_prev * num_feat_current];
			v_biases = new double[num_feat_current];

			aggregatedFeatures = new double[num_nodes * num_feat_current];
			preActivatedFeatures = new double[num_nodes * num_feat_current];
			postActivatedFeatures = new double[num_nodes * num_feat_current];
			grad_pre_act_output = new double[num_nodes * num_feat_current];
		}
};

__global__ void initializeLayers_cuda(Layer *layers, int *numFeaturesPerLayer, int num_nodes){
	int idx = blockIdx.x * blockDim.x + threadIdx.x;
	if (idx == 0){
		layers[idx].num_features =  numFeaturesPerLayer[idx];
		//  layers[0].preActivatedFeatures = new double *[gnn.getGraph().num_nodes()];
		//   layers[0].postActivatedFeatures = new double *[gnn.getGraph().num_nodes()];
		layers[idx].preActivatedFeatures = new double[num_nodes * numFeaturesPerLayer[idx]];
		layers[idx].postActivatedFeatures = new double[num_nodes * numFeaturesPerLayer[idx]];
	}
	layers[idx].initializeLayers(idx, numFeaturesPerLayer[idx], numFeaturesPerLayer[idx-1], num_nodes);
}

void initializeLayers_cuda(GNN &gnn, vector<int> numFeaturesPerLayer, char *transformation){
	int num_layers = numNodespreLayer.size() - 1;
	Layer *layers;
	if(strcmp(transformation, "xaviers") == 0){
		transformType = 1;
	}
	else if(strcmp(transformation, "he") == 0){
		transformType = 2;
	}
	cudaMallocManaged(&layers, num_layers * sizeof(Layer));
	//create numFeaturesperLayer in cuda and copy
	int *d_numFeaturesperLayer;
	cudaMallocManaged(&d_numFeaturesperLayer, num_layers * sizeof(int));
	cudaMemcpy(h_numFeaturesperLayer, numFeaturesperLayer.data(), num_layers * sizeof(int), cudaMemcpyHostToDevice);
	//initialize layers
	initializeLayers_cuda<<<1, num_layers>>>(layers, h_numFeaturesperLayer, num_nodes);
	cudaDeviceSynchronize();

}

gnn.forward(a,b,c) -> forward_cuda(gnn, a ,b, c)

void forward(){


	aggregation<<<1,1>>>();

	NNoperation<<<1,1>>>();



}



void optimizer(char *algorithm, char * optimizer_type, int batch_size, double learning_rate, double weight_decay){

	
	if(strcmp(algorithm,"GraphSage")==0){
		forward_Sage(batch_size);
	}
	else if(strcmp(algorithm,"GCN")==0){
		forward_GCN();
	}
	else if(strcmp(algorithm, "GIN")==0){
		forward_GIN();
	}

	if(strcmp(optimizer_type, "adam") == 0){
		adam_optimizer(learning_rate, weight_decay);
	}
	else if(strcmp(optimizer_type, "sgd") == 0){
		sgd_optimizer(learning_rate, weight_decay);
	}
	else if(strcmp(optimizer_type, "rmsprop") == 0){
		rmsprop_optimizer(learning_rate, weight_decay);
	}


}


__global__ void GCN_aggregate_cuda(int numNodes, int numFeatures, int* rowPtr, int* colIdx, double* edgeWeight, double* postActivatedFeatures, double* aggregatedFeatures)
{
    int node = blockIdx.x * blockDim.x + threadIdx.x;
    if (node >= numNodes) return; 

    for (int i = 0; i < numFeatures; ++i) {
        aggregatedFeatures[node * numFeatures + i] = 0.0f;
    }

    for (int edge = rowPtr[node]; edge < rowPtr[node + 1]; ++edge) {		    // Iterate over the neighbors of the node

        int neighbor = colIdx[edge];
        double weight = edgeWeight[edge];
        
        for (int i = 0; i < numFeatures; ++i) {				        // Aggregate features from neighbor node
            atomicAdd(&aggregatedFeatures[node * numFeatures + i], 
                      postActivatedFeatures[neighbor * numFeatures + i] * weight);
        }
    }
}

__global__ void softmaxKernel(double* data, int numFeatures)
{
    extern __shared__ double temp[];
    int tid = threadIdx.x;

    
    temp[tid] = expf(data[tid]);
    __syncthreads();

    
    double sum = 0.0f;
    for (int i = 0; i < numFeatures; ++i)
        sum += temp[i];
    
    __syncthreads();
    
    data[tid] = temp[tid] / sum;
}


__global__ void Activation_cuda(double* preActivated, double* postActivated, double* bias, int numFeatures, int initType, int totalLayers, int currentLayer)
{
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < numFeatures)
    {
        // Add bias
        preActivated[i] += bias[i];

        // Apply activation
        if (initType == 2 && currentLayer<totalLayers)
            postActivated[i] = fmaxf(0.0f, preActivated[i]);  // ReLU
        if (initType == 1 && currentLayer<totalLayers)
            postActivated[i] = tanhf(preActivated[i]);  // Tanh
	}
}


void forward(){

	double* d_aggregatedFeatures = layers[layerNumber].aggregatedFeatures[node];  // Aggregated features (GPU memory)
    double* d_weights = layers[layerNumber].weights;  // Weights matrix 
    double* d_preActivatedFeatures = layers[layerNumber].preActivatedFeatures[node];  // Pre-activation features
    double* d_bias = layers[layerNumber].bias;  // Bias
    double* d_postActivatedFeatures = layers[layerNumber].postActivatedFeatures[node];  // Post-activation features
	int numBlocks = (numNodes + 1024 - 1) / 1024;
	GCN_aggregate_cuda<<<numBlocks, 1024>>>(numNodes,numFeatures, d_rowPtr, d_colIdx, d_edgeWeight, d_postActivatedFeatures, d_aggregatedFeatures)
	cudaDeviceSynchronize();
//	cusparse()
	int blockSize = 256;
    int gridSize = (numFeatures + blockSize - 1) / blockSize;
	Activation_cuda<<<gridSize, blockSize>>>(d_preActivatedFeatures, d_postActivatedFeatures, d_bias, numFeatures,activationType);
	 if (layerNumber == layers.size() - 1)
    {
        softmaxKernel<<<1, numFeatures, numFeatures * sizeof(double)>>>(d_postActivatedFeatures, numFeatures);
    }
}


__global__ void computeGradientsOutputLayer(double *y_pred, int *y_true, double *grad_output, int num_nodes, int num_classes)
{
    int nodeIdx = blockIdx.x * blockDim.x + threadIdx.x;

    if (nodeIdx < num_nodes) {
        int label = y_true[nodeIdx];  
        for (int j = 0; j < num_classes; ++j) {
            grad_output[nodeIdx * num_classes + j] = y_pred[nodeIdx * num_classes + j] - (label == j ? 1.0f : 0.0f);
        }
    }
}

__global__ void computeGradientsIntermediateLayer(double *grad_next_layer, double *weights_next_layer, double *grad_current_layer, double *pre_activated_features, int num_nodes, int num_features, int next_layer_features, int activation_type)
{
    int nodeIdx = blockIdx.x * blockDim.x + threadIdx.x;

    if (nodeIdx < num_nodes) {
        for (int i = 0; i < num_features; ++i) {
            double grad_sum = 0.0f;
            for (int j = 0; j < next_layer_features; ++j) {
                grad_sum += grad_next_layer[nodeIdx * next_layer_features + j] * weights_next_layer[i * next_layer_features + j];
            }

            if (activation_type == 1) {  // Tanh
                grad_current_layer[nodeIdx * num_features + i] = grad_sum * (1 - pre_activated_features[nodeIdx * num_features + i] * pre_activated_features[nodeIdx * num_features + i]);
            } else if (activation_type == 2) {  // ReLU
                grad_current_layer[nodeIdx * num_features + i] = (pre_activated_features[nodeIdx * num_features + i] > 0) ? grad_sum : 0;
            }
        }
    }
}
__global__ void computeWeightBiasGradients(double *aggregated_features, double *grad_pre_act_output, double *grad_weights, double *grad_bias, int num_nodes, int num_input_features, int num_output_features)
{
    int inputIdx = blockIdx.x * blockDim.x + threadIdx.x;

    if (inputIdx < num_input_features) {
        for (int j = 0; j < num_output_features; ++j) {
            double weight_grad_sum = 0.0f;
            double bias_grad_sum = 0.0f;
            
            for (int nodeIdx = 0; nodeIdx < num_nodes; ++nodeIdx) {
                weight_grad_sum += aggregated_features[nodeIdx * num_input_features + inputIdx] * grad_pre_act_output[nodeIdx * num_output_features + j];
                bias_grad_sum += grad_pre_act_output[nodeIdx * num_output_features + j];
            }

            grad_weights[inputIdx * num_output_features + j] = weight_grad_sum;
            if (inputIdx == 0) {  
                grad_bias[j] = bias_grad_sum;
            }
        }
    }
}
__global__ void clipGradients(double *grad_weights, double *grad_bias, int num_input_features, int num_output_features, double clip_value)
{
    int weightIdx = blockIdx.x * blockDim.x + threadIdx.x;

    if (weightIdx < num_input_features * num_output_features) {
        if (abs(grad_weights[weightIdx]) > clip_value) {
            grad_weights[weightIdx] = clip_value * (grad_weights[weightIdx] > 0 ? 1.0f : -1.0f);
        }
    }

    int biasIdx = threadIdx.x;
    if (biasIdx < num_output_features) {
        if (abs(grad_bias[biasIdx]) > clip_value) {
            grad_bias[biasIdx] = clip_value * (grad_bias[biasIdx] > 0 ? 1.0f : -1.0f);
        }
    }
}


void backPropagation_cuda(GNN &gnn, int layerNumber, int numNodes, int numInputFeatures, int numOutputFeatures) {
    double *d_y_pred, *d_grad_output, *d_weights, *d_preActivatedFeatures, *d_aggregatedFeatures;
    int *d_y_true;
    
    if (layerNumber == gnn.numLayers() - 1) {
        int threads = 1024;
        int blocks = (numNodes + threads - 1) / threads;
        computeGradientsOutputLayer<<<blocks, threads>>>(d_y_pred, d_y_true, d_grad_output, numNodes, gnn.numClasses());
    } else {
        int threads = 1024;
        int blocks = (numNodes + threads - 1) / threads;
        computeGradientsIntermediateLayer<<<blocks, threads>>>(d_grad_output, d_weights, d_grad_output, d_preActivatedFeatures, numNodes, numInputFeatures, numOutputFeatures, gnn.initType());
    }

    int threads = 1024;
    int blocks = (numInputFeatures + threads - 1) / threads;
    computeWeightBiasGradients<<<blocks, threads>>>(d_aggregatedFeatures, d_grad_output, d_weights, d_bias, numNodes, numInputFeatures, numOutputFeatures);
    clipGradients<<<blocks, threads>>>(d_weights, d_bias, numInputFeatures, numOutputFeatures, 3.0);
}
