#include<iostream>
#include<cuda_runtime.h>

class GraphNeuralNetworks{
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
		__device__ GraphNeuralNetworks() : num_features(0), weights(nullptr), bias(nullptr), epsilon(0.01), grad_epsilon(0.0), aggregatedFeatures(nullptr), preActivatedFeatures(nullptr), postActivatedFeatures(nullptr),grad_pre_act_output(nullptr), grad_weights(nullptr), grad_bias(nullptr), m_weights(nullptr), m_biases(nullptr), m_epsilon(0.0f), v_epsilon(0.0f), v_weights(nullptr), v_biases(nullptr) {}


		__device__ void initializeLayers(int32_t layer_num, int32_t num_feat_current, int32_t num_feat_next){
			weights = new double[num_feat_current + num_feat_next];
			bias = new double[num_feat_next];
			grad_weights = new double[num_feat_current + num_feat_next];
			grad_bias = new double[num_feat_next];
			m_weights = new double[num_feat_current + num_feat_next];
			m_biases = new double[num_feat_next];	
			v_weights = new double[num_feat_current + num_feat_next];
		}
};

__global__ void initializeLayers_cuda(GraphNeuralNetworks *layers, int32_t currentSize, int32_t nextSize){
	int idx = blockIdx.x * blockDim.x + threadIdx.x;
	layers[idx].initializeLayers(idx, currentSize, nextSize);
}

void initializeLayers_cuda(vector<int> numnodespreLayer, char *transformation){
	int num_layers = numNodespreLayer.size() - 1;
	GraphNeuralNetworks *layers;
	if(strcmp(transformation, "xaviers") == 0){
		transformType = 1;
	}
	else if(strcmp(transformation, "he") == 0){
		transformType = 2;
	}
	cudaMallocManaged(&layers, num_layers * sizeof(GraphNeuralNetworks));
	for(int i = 1;i<num_layers-1;i++){
	initializeLayers_cuda<<<1, 1>>>(layers, numNodesperLayer[i], numNodesperLayer[i+1]);
	}
	cudaDeviceSynchronize();

	

}

void forwardProp_cuda(int32_t layer){
	GraphNeuralNetworks *layers;
	aggregatedGCN_cuda<<<numNodes, numFeatures>>>(layers, layer);
	cudaDeviceSynchronize();
	GCN_NN<<<numNodes, numFeatures>>>(layers, layer);
	cudaDeviceSynchronize();
}