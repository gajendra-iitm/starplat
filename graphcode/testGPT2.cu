#include <iostream>
#include <curand_kernel.h>
#include <cuda_runtime.h>
#include <vector>
#include "graph.cpp"
using namespace std;

env environment("cuda", "static", "test.cu");

class Layer
{
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

	Layer() : num_features(0), weights(nullptr), bias(nullptr), epsilon(0.01), grad_epsilon(0.0), aggregatedFeatures(nullptr), preActivatedFeatures(nullptr), postActivatedFeatures(nullptr), grad_pre_act_output(nullptr), grad_weights(nullptr), grad_bias(nullptr), m_weights(nullptr), m_biases(nullptr), m_epsilon(0.0f), v_epsilon(0.0f), v_weights(nullptr), v_biases(nullptr) {}

	__device__ void xaviers(double *weights, int num_feat_current, int num_feat_prev)
	{
		double x = sqrt(6.0f / (num_feat_current + num_feat_prev));
		curandState state;
		printf("num_feat_current: %d\n", num_feat_current);
		curand_init(3566, 5, 0, &state);
		for (int i = 0; i < num_feat_current * num_feat_prev; i++)
		{
			curandState state;
			curand_init(3566, i, 0, &state);
			float random_value = curand_uniform(&state) * x;
			weights[i] = random_value;
			// printf("%f\n",weights[i]);
		}
	}

	__device__ void he(double *weights, int num_feat_current, int num_feat_prev)
	{
		double x = sqrt(6.0f / (num_feat_prev));
		curandState state;
		printf("num_feat_current: %d\n", num_feat_current);
		curand_init(3566, 5, 0, &state);
		for (int i = 0; i < num_feat_current * num_feat_prev; i++)
		{
			curandState state;
			curand_init(3566, i, 0, &state);
			float random_value = curand_uniform(&state) * x;
			weights[i] = random_value;
			// printf("%f\n",weights[i]);
		}
	}
};

class cudaVars
{
public:
	Layer *layers;
	int *d_nodesPtr;
	int *d_edgesList;
	float *d_edgeWeights;
};

cudaVars globalCudaVar;

__global__ void initializeLayerFields(Layer *layers, int num_layers, int *num_features, double *d_features, int num_nodes, int initialization_type)
{
	int i = threadIdx.x;
	if (i == 0)
	{
		layers[i].num_features = num_features[i];
		for (int j = 0; j < num_features[0] * num_nodes; j++)
		{
			layers[i].preActivatedFeatures[j] = 0;
			layers[i].postActivatedFeatures[j] = d_features[j];
		}
	}

	else if (i < num_layers)
	{
		layers[i].num_features = num_features[i];
		if (initialization_type == 1)
		{
			layers[i].xaviers(layers[i].weights, num_features[i - 1], num_features[i]);
		}
		else if (initialization_type == 2)
		{
			layers[i].he(layers[i].weights, num_features[i - 1], num_features[i]);
		}
		for (int j = 0; j < num_features[i]; j++)
		{
			layers[i].bias[j] = 0.5;
			layers[i].grad_bias[j] = 0;
			layers[i].m_biases[j] = 0;
			layers[i].v_biases[j] = 0;
		}

		for (int j = 0; j < num_features[i - 1] * num_features[i]; j++)
		{
			layers[i].grad_weights[j] = 0;
			layers[i].m_weights[j] = 0;
			layers[i].v_weights[j] = 0;
		}

		for (int j = 0; j < num_features[i] * num_nodes; j++)
		{
			layers[i].aggregatedFeatures[j] = 0;
			layers[i].preActivatedFeatures[j] = 0;
			layers[i].postActivatedFeatures[j] = 0;
			layers[i].grad_pre_act_output[j] = 0;
		}
	}
}

void initializeLayers_cuda(GNN &gnn, Layer *d_layers, vector<int> numFeaturesPerLayer, double *d_features, int num_nodes, const char *transformation)
{
	int *d_numFeaturesPerLayer;
	cudaMalloc(&d_numFeaturesPerLayer, numFeaturesPerLayer.size() * sizeof(int));
	cudaMemcpy(d_numFeaturesPerLayer, numFeaturesPerLayer.data(), numFeaturesPerLayer.size() * sizeof(int), cudaMemcpyHostToDevice);

	int initialization_type;
	if (transformation == "xaviers")
	{
		initialization_type = 1;
	}
	else if (transformation == "he")
	{
		initialization_type = 2;
	}

	initializeLayerFields<<<1, numFeaturesPerLayer.size()>>>(d_layers, numFeaturesPerLayer.size(), d_numFeaturesPerLayer, d_features, num_nodes, initialization_type);
	cudaDeviceSynchronize();

	cudaError_t err = cudaGetLastError();
	if (err != cudaSuccess)
	{
		printf("CUDA Error: %s\n", cudaGetErrorString(err));
	}
	else
	{
		printf("Success\n");
	}

	globalCudaVar.layers = d_layers;

	cudaMalloc(&globalCudaVar.d_nodesPtr, gnn.getGraph().num_nodes() * sizeof(int));
	cudaMemcpy(globalCudaVar.d_nodesPtr, gnn.getGraph().getNodesptr(), gnn.getGraph().num_nodes() * sizeof(int), cudaMemcpyHostToDevice);

	cudaMalloc(&globalCudaVar.d_edgesList, gnn.getGraph().num_edges() * sizeof(int));
	cudaMemcpy(globalCudaVar.d_edgesList, gnn.getGraph().getEdgesList(), gnn.getGraph().num_edges() * sizeof(int), cudaMemcpyHostToDevice);

	cudaMalloc(&globalCudaVar.d_edgeWeights, gnn.getGraph().num_edges() * sizeof(float));
	cudaMemcpy(globalCudaVar.d_edgeWeights, gnn.getGraph().getEdgeLen(), gnn.getGraph().num_edges() * sizeof(float), cudaMemcpyHostToDevice);

	// copy the features to a double  and print the features of node 4

	// copy the weights of layer 1 to a double array and print the first 10 weights
	//  double *weights = new double[numFeaturesPerLayer[0] * numFeaturesPerLayer[1]];
	//  cudaMemcpy(weights, h_layers[2].weights, numFeaturesPerLayer[0] * numFeaturesPerLayer[1] * sizeof(double), cudaMemcpyDeviceToHost);
	//  for (int i = 0; i < 10; i++)
	//  {
	//  	cout << weights[i] << " ";
	//  }
}

__global__ void GCN_cuda(Layer* layers, int batchNumber, int batchSize, int *V, int *E, float *edgeWeights, int layerNumber)
{
	int tid = threadIdx.x + blockIdx.x * blockDim.x;
	if (tid < batchSize)
	{
		int idx = batchNumber * batchSize + tid;
		for (int i = 0; i < layers[layerNumber].num_features; i++)
		{
			layers[layerNumber].aggregatedFeatures[idx * layers[layerNumber].num_features + i] = 0;
		}

		for (int j = V[idx]; j < V[idx + 1]; j++)
		{
			int neighbor = E[j];
			double weight = edgeWeights[j];
			for (int i = 0; i < layers[layerNumber].num_features; i++)
			{
				layers[layerNumber].aggregatedFeatures[idx * layers[layerNumber].num_features + i] += layers[layerNumber - 1].postActivatedFeatures[neighbor * layers[layerNumber].num_features + i] * weight;
			}
		}

		printf("Node %d: aggregatedFeatures = %f\n", idx, layers[layerNumber].aggregatedFeatures[idx * layers[layerNumber].num_features]);
	}
}

__global__ void NeuralNetworkOperation(Layer* layers, int batchNumber, int batchSize, int totalLayers, int layerNumber, int activationType)
{
	int tid = threadIdx.x + blockIdx.x * blockDim.x;
	if (tid < batchSize)
	{
		// printf("tid: %d\n", tid);

		int idx = batchNumber * batchSize + tid;
		if (layerNumber < totalLayers - 1)
			for (int i = 0; i < layers[layerNumber].num_features; i++)
			{
				float sum = 0.0f;

				for (int j = 0; j < layers[layerNumber - 1].num_features; j++)
				{
					sum += layers[layerNumber].weights[j * layers[layerNumber].num_features + i] *
						   layers[layerNumber].aggregatedFeatures[idx * layers[layerNumber - 1].num_features + j];
				}

				layers[layerNumber].preActivatedFeatures[idx * layers[layerNumber + 1].num_features + i] = sum;
				if (activationType == 1)
				{
					layers[layerNumber].postActivatedFeatures[idx * layers[layerNumber + 1].num_features + i] =
						fmaxf(0.0f, sum);
				}
				else if (activationType == 2)
				{
					layers[layerNumber].postActivatedFeatures[idx * layers[layerNumber + 1].num_features + i] =
						tanh(sum);
				}
				printf("Node %d: preActivatedFeatures = %f\n", idx, layers[layerNumber].preActivatedFeatures[idx * layers[layerNumber + 1].num_features + i]);

				printf("Node %d: postActivatedFeatures = %f\n", idx, layers[layerNumber].postActivatedFeatures[idx * layers[layerNumber + 1].num_features + i]);
			}
	}
}

void forwardPass_cuda(GNN &gnn, int layerNumber, int batchNumber, int batchSize, int activationType)
{
	int numNodesInBatch = batchSize;
	int threadsPerBlock = 1024;
	int blocksPerGrid = (numNodesInBatch + threadsPerBlock - 1) / threadsPerBlock;
	cudaStream_t stream1, stream2;

	Layer* layers = globalCudaVar.layers;

	// GCN_cuda<<<blocksPerGrid, threadsPerBlock, 0, stream1>>>(layers, numNodes, gnn.getGraph().getNodesptr(), gnn.getGraph().getEdgesList(), gnn.getGraph().getEdgeLen(), layerNumber, node, activationType);
	GCN_cuda<<<blocksPerGrid, threadsPerBlock>>>(layers, batchNumber, batchSize, globalCudaVar.d_nodesPtr, globalCudaVar.d_edgesList, globalCudaVar.d_edgeWeights, layerNumber);
	cudaDeviceSynchronize();

	// Launch kernels  to overlap execution
	NeuralNetworkOperation<<<blocksPerGrid, threadsPerBlock>>>(layers,batchNumber, batchSize, 4, layerNumber, activationType);
	cudaDeviceSynchronize();

	cudaError_t err = cudaGetLastError();
	if (err != cudaSuccess)
	{
		printf("CUDA ffError: %s\n", cudaGetErrorString(err));
	}
	else
	{
		printf("Suffccess\n");
	}
}

// __global__ void computeGradientsOutputLayer(double *y_pred, int *y_true, double *grad_output, int num_nodes, int num_classes)
// {

// 	int nodeIdx = blockIdx.x * blockDim.x + threadIdx.x;

// 	if (nodeIdx < num_nodes)
// 	{
// 		int label = y_true[nodeIdx];
// 		for (int j = 0; j < num_classes; ++j)
// 		{
// 			grad_output[nodeIdx * num_classes + j] = y_pred[nodeIdx * num_classes + j] - (label == j ? 1.0f : 0.0f);
// 		}
// 	}
// }

// __global__ void computeGradientsIntermediateLayer(double *grad_next_layer, double *weights_next_layer, double *grad_current_layer, double *pre_activated_features, int num_nodes, int num_features, int next_layer_features, int activation_type)
// {
// 	int nodeIdx = blockIdx.x * blockDim.x + threadIdx.x;

// 	if (nodeIdx < num_nodes)
// 	{
// 		for (int i = 0; i < num_features; ++i)
// 		{
// 			double grad_sum = 0.0f;
// 			for (int j = 0; j < next_layer_features; ++j)
// 			{
// 				grad_sum += grad_next_layer[nodeIdx * next_layer_features + j] * weights_next_layer[i * next_layer_features + j];
// 			}

// 			if (activation_type == 1)
// 			{ // Tanh
// 				grad_current_layer[nodeIdx * num_features + i] = grad_sum * (1 - pre_activated_features[nodeIdx * num_features + i] * pre_activated_features[nodeIdx * num_features + i]);
// 			}
// 			else if (activation_type == 2)
// 			{ // ReLU
// 				grad_current_layer[nodeIdx * num_features + i] = (pre_activated_features[nodeIdx * num_features + i] > 0) ? grad_sum : 0;
// 			}
// 		}
// 	}
// }
// __global__ void computeWeightBiasGradients(double *aggregated_features, double *grad_pre_act_output, double *grad_weights, double *grad_bias, int num_nodes, int num_input_features, int num_output_features)
// {
// 	int inputIdx = blockIdx.x * blockDim.x + threadIdx.x;

// 	if (inputIdx < num_input_features)
// 	{
// 		for (int j = 0; j < num_output_features; ++j)
// 		{
// 			double weight_grad_sum = 0.0f;
// 			double bias_grad_sum = 0.0f;

// 			for (int nodeIdx = 0; nodeIdx < num_nodes; ++nodeIdx)
// 			{
// 				weight_grad_sum += aggregated_features[nodeIdx * num_input_features + inputIdx] * grad_pre_act_output[nodeIdx * num_output_features + j];
// 				bias_grad_sum += grad_pre_act_output[nodeIdx * num_output_features + j];
// 			}

// 			grad_weights[inputIdx * num_output_features + j] = weight_grad_sum;
// 			if (inputIdx == 0)
// 			{
// 				grad_bias[j] = bias_grad_sum;
// 			}
// 		}
// 	}
// }
// __global__ void clipGradients(double *grad_weights, double *grad_bias, int num_input_features, int num_output_features, double clip_value)
// {
// 	int weightIdx = blockIdx.x * blockDim.x + threadIdx.x;

// 	if (weightIdx < num_input_features * num_output_features)
// 	{
// 		if (abs(grad_weights[weightIdx]) > clip_value)
// 		{
// 			grad_weights[weightIdx] = clip_value * (grad_weights[weightIdx] > 0 ? 1.0f : -1.0f);
// 		}
// 	}

// 	int biasIdx = threadIdx.x;
// 	if (biasIdx < num_output_features)
// 	{
// 		if (abs(grad_bias[biasIdx]) > clip_value)
// 		{
// 			grad_bias[biasIdx] = clip_value * (grad_bias[biasIdx] > 0 ? 1.0f : -1.0f);
// 		}
// 	}
// }

// void backPropagation_cuda(GNN &gnn, int layerNumber, int numNodes, int numInputFeatures, int numOutputFeatures)
// {
// 	double *d_y_pred, *d_grad_output, *d_weights, *d_preActivatedFeatures, *d_aggregatedFeatures, *d_bias;
// 	int *d_y_true;
// 	int totalLayers;
// 	if (layerNumber == totalLayers - 1)
// 	{
// 		int threads = 1024;
// 		int blocks = (numNodes + threads - 1) / threads;
// 		computeGradientsOutputLayer<<<blocks, threads>>>(d_y_pred, d_y_true, d_grad_output, numNodes, gnn.numClasses());
// 	}
// 	else
// 	{
// 		int threads = 1024;
// 		int blocks = (numNodes + threads - 1) / threads;
// 		computeGradientsIntermediateLayer<<<blocks, threads>>>(d_grad_output, d_weights, d_grad_output, d_preActivatedFeatures, numNodes, numInputFeatures, numOutputFeatures, gnn.initType());
// 	}

// 	int threads = 1024;
// 	int blocks = (numInputFeatures + threads - 1) / threads;
// 	computeWeightBiasGradients<<<blocks, threads>>>(d_aggregatedFeatures, d_grad_output, d_weights, d_bias, numNodes, numInputFeatures, numOutputFeatures);
// 	clipGradients<<<blocks, threads>>>(d_weights, d_bias, numInputFeatures, numOutputFeatures, 3.0);
// }

int main()
{
	vector<int> numFeaturesPerLayer = {16, 12, 8};
	const char *transformation = "xaviers";
	graph G("/home/anubhav/new_q/newww_tha/Starplat-GNN/graphcode/sample_graphs/sample_graph/sample_graph.txt");
	G.parseGraph();

	GNN gnn(G, "/home/anubhav/new_q/newww_tha/Starplat-GNN/graphcode/sample_graphs/sample_graph/sample_graph_feat.txt", "/home/anubhav/new_q/newww_tha/Starplat-GNN/graphcode/sample_graphs/sample_graph/sample_graph_labels.txt");
	gnn.gcnPreprocessing();
	cout << "preprosessing done" << endl;

	int num_layers = numFeaturesPerLayer.size();

	Layer *d_layers;
	cudaMalloc(&d_layers, num_layers * sizeof(Layer));

	Layer *h_layers = new Layer[num_layers];
	for (int i = 0; i < num_layers; i++)
	{
		h_layers[i] = Layer();
		cudaMalloc(&h_layers[i].weights, numFeaturesPerLayer[(i > 1 ? i - 1 : i)] * numFeaturesPerLayer[i] * sizeof(double));
		cudaMalloc(&h_layers[i].bias, numFeaturesPerLayer[i] * sizeof(double));
		cudaMalloc(&h_layers[i].grad_weights, numFeaturesPerLayer[(i > 1 ? i - 1 : i)] * numFeaturesPerLayer[i] * sizeof(double));
		cudaMalloc(&h_layers[i].grad_bias, numFeaturesPerLayer[i] * sizeof(double));
		cudaMalloc(&h_layers[i].m_weights, numFeaturesPerLayer[(i > 1 ? i - 1 : i)] * numFeaturesPerLayer[i] * sizeof(double));
		cudaMalloc(&h_layers[i].m_biases, numFeaturesPerLayer[i] * sizeof(double));
		cudaMalloc(&h_layers[i].v_weights, numFeaturesPerLayer[(i > 1 ? i - 1 : i)] * numFeaturesPerLayer[i] * sizeof(double));
		cudaMalloc(&h_layers[i].v_biases, numFeaturesPerLayer[i] * sizeof(double));

		cudaMalloc(&h_layers[i].aggregatedFeatures, gnn.getGraph().num_nodes() * numFeaturesPerLayer[i] * sizeof(double));
		cudaMalloc(&h_layers[i].preActivatedFeatures, gnn.getGraph().num_nodes() * numFeaturesPerLayer[i] * sizeof(double));
		cudaMalloc(&h_layers[i].postActivatedFeatures, gnn.getGraph().num_nodes() * numFeaturesPerLayer[i] * sizeof(double));
		cudaMalloc(&h_layers[i].grad_pre_act_output, gnn.getGraph().num_nodes() * numFeaturesPerLayer[i] * sizeof(double));

		cudaMemcpy(&d_layers[i], &h_layers[i], sizeof(Layer), cudaMemcpyHostToDevice);
	}
	vector<vector<double>> feats = gnn.getFeatures();
	// convert it to a 1D array
	vector<double> flat_feats;
	for (int i = 0; i < feats.size(); i++)
	{
		for (int j = 0; j < feats[i].size(); j++)
		{
			flat_feats.push_back(feats[i][j]);
		}
	}

	double *d_features;
	cudaMalloc(&d_features, gnn.getGraph().num_nodes() * feats[0].size() * sizeof(double));
	// .data of vector of vector is a 1D array
	cudaMemcpy(d_features, flat_feats.data(), gnn.getGraph().num_nodes() * feats[0].size() * sizeof(double), cudaMemcpyHostToDevice);

	initializeLayers_cuda(gnn, d_layers, numFeaturesPerLayer, d_features, gnn.getGraph().num_nodes(), transformation);
	cudaDeviceSynchronize();
	double *features = new double[gnn.getGraph().num_nodes() * feats[0].size()];
	cudaMemcpy(features, d_features, gnn.getGraph().num_nodes() * feats[0].size() * sizeof(double), cudaMemcpyDeviceToHost);

	printf("\n\ndone\n\n");
	forwardPass_cuda(gnn, 1, 0, 3, 1);
	//  double *features = new double[num_nodes * feats[0].size()];
	//  cudaMemcpy(features, d_features, num_nodes * feats[0].size() * sizeof(double), cudaMemcpyDeviceToHost);
	//  for (int i = 0; i < feats[0].size(); i++)
	//  {
	//  	cout << features[4 * feats[0].size() + i] << " ";
	//  }

	// print features of node 4
	double *featuress = new double[gnn.getGraph().num_nodes() * feats[0].size()];
	cudaMemcpy(featuress, h_layers[1].preActivatedFeatures, gnn.getGraph().num_nodes() * feats[0].size() * sizeof(double), cudaMemcpyDeviceToHost);

	return 0;
}
