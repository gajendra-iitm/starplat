#include <iostream>
#include <curand_kernel.h>
#include <cuda_runtime.h>
#include <vector>
#include "graph.cpp"
#define GRADIENT_NORM_CLIP_VALUE 3.0f
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
		// printf("num_feat_current: %d\n", num_feat_current);
		for (int i = 0; i < num_feat_current * num_feat_prev; i++)
		{
			curandState state;
			curand_init(3244, i, 0, &state);
			weights[i] = curand_uniform(&state) * x;
			// printf("%f\n",weights[i]);
		}
	}

	__device__ void he(double *weights, int num_feat_current, int num_feat_prev)
	{
		double x = sqrt(2.0f / (num_feat_prev));
		curandState state;
		// printf("num_feat_current: %d\n", num_feat_current);
		curand_init(42, 5, 0, &state);
		for (int i = 0; i < num_feat_current * num_feat_prev; i++)
		{
			curandState state;
			curand_init(42, i, 0, &state);
			double random_value = curand_uniform(&state) * x;
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
	int *d_y_true;
};

cudaVars globalCudaVar;

__global__ void initializeLayerFields(Layer *layers, int num_layers, int *num_features, double *d_features, int num_nodes, int initialization_type)
{
	int i = threadIdx.x;
	if (i == 0)
	{
		layers[i].num_features = num_features[i];
		// printf("num_features: %d and i = %d\n", layers[i].num_features, i);
		for (int j = 0; j < num_nodes; j++)
		{
			for (int k = 0; k < layers[i].num_features; k++)
			{
				layers[i].preActivatedFeatures[j * layers[i].num_features + k] = 0;
				layers[i].postActivatedFeatures[j * layers[i].num_features + k] = d_features[j * layers[i].num_features + k];
			}
		}
	}

	else if (i < num_layers)
	{
		layers[i].num_features = num_features[i];
		// if (initialization_type == 1)
		// {
		// 	layers[i].xaviers(layers[i].weights, num_features[i], num_features[i - 1]);
		// }
		// else if (initialization_type == 2)
		// {
		// 	layers[i].he(layers[i].weights, num_features[i - 1], num_features[i]);
		// }

		for (int j = 0; j < num_features[i]; j++)
		{
			// layers[i].bias[j] = 0.5;
			layers[i].bias[j] = 0;
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

		for (int j = 0; j < num_features[i - 1] * num_nodes; j++)
		{
			layers[i].aggregatedFeatures[j] = 0;
		}

		for (int j = 0; j < num_features[i] * num_nodes; j++)
		{
			layers[i].preActivatedFeatures[j] = 0;
			layers[i].postActivatedFeatures[j] = 0;
			layers[i].grad_pre_act_output[j] = 0;
		}
	}
}

__global__ void weights_copy_per_layer(Layer *layers, int layerNumber, double *weights, int num_features_current, int num_features_prev)
{
	int i = threadIdx.x + blockIdx.x * blockDim.x;
	if (i < num_features_current * num_features_prev)
	{
		layers[layerNumber].weights[i] = weights[i];
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
		for (int i = 1; i < numFeaturesPerLayer.size(); i++)
		{
			double xavier = sqrt(6.0 / (numFeaturesPerLayer[i] + numFeaturesPerLayer[i - 1]));
			std::random_device rd; // Obtain a random number from hardware
			std::mt19937 gen(42);  // Seed the generator
			std::uniform_real_distribution<> dist(0, xavier);

			double *weights = new double[numFeaturesPerLayer[i] * numFeaturesPerLayer[i - 1]];
			double *d_weights;
			cudaMalloc(&d_weights, numFeaturesPerLayer[i] * numFeaturesPerLayer[i - 1] * sizeof(double));
			for (int j = 0; j < numFeaturesPerLayer[i] * numFeaturesPerLayer[i - 1]; j++)
			{
				weights[j] = dist(gen);
			}
			cudaMemcpy(d_weights, weights, numFeaturesPerLayer[i] * numFeaturesPerLayer[i - 1] * sizeof(double), cudaMemcpyHostToDevice);

			int threadsPerBlock = 1024;
			int blocksPerGrid = (numFeaturesPerLayer[i] * numFeaturesPerLayer[i - 1] + threadsPerBlock - 1) / threadsPerBlock;
			weights_copy_per_layer<<<blocksPerGrid, threadsPerBlock>>>(d_layers, i, d_weights, numFeaturesPerLayer[i], numFeaturesPerLayer[i - 1]);
			cudaDeviceSynchronize();
			cudaError_t err = cudaGetLastError();
			if (err != cudaSuccess)
			{
				printf("CUDA Errorxaviers: %s\n", cudaGetErrorString(err));
			}
			delete weights;
		}
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

	globalCudaVar.layers = d_layers;

	cudaMalloc(&globalCudaVar.d_nodesPtr, gnn.getGraph().num_nodes() * sizeof(int));
	cudaMemcpy(globalCudaVar.d_nodesPtr, gnn.getGraph().getNodesptr(), gnn.getGraph().num_nodes() * sizeof(int), cudaMemcpyHostToDevice);

	cudaMalloc(&globalCudaVar.d_edgesList, gnn.getGraph().num_edges() * sizeof(int));
	cudaMemcpy(globalCudaVar.d_edgesList, gnn.getGraph().getEdgesList(), gnn.getGraph().num_edges() * sizeof(int), cudaMemcpyHostToDevice);

	cudaMalloc(&globalCudaVar.d_edgeWeights, gnn.getGraph().num_edges() * sizeof(float));
	cudaMemcpy(globalCudaVar.d_edgeWeights, gnn.getGraph().getEdgeLen(), gnn.getGraph().num_edges() * sizeof(float), cudaMemcpyHostToDevice);

	cudaMalloc(&globalCudaVar.d_y_true, gnn.getGraph().num_nodes() * sizeof(int));
	cudaMemcpy(globalCudaVar.d_y_true, gnn.getLabels().data(), gnn.getGraph().num_nodes() * sizeof(int), cudaMemcpyHostToDevice);

	// copy the features to a double  and print the features of node 4

	// copy the weights of layer 1 to a double array and print the first 10 weights
	//  double *weights = new double[numFeaturesPerLayer[0] * numFeaturesPerLayer[1]];
	//  cudaMemcpy(weights, h_layers[2].weights, numFeaturesPerLayer[0] * numFeaturesPerLayer[1] * sizeof(double), cudaMemcpyDeviceToHost);
	//  for (int i = 0; i < 10; i++)
	//  {
	//  	cout << weights[i] << " ";
	//  }
}

__global__ void GCN_aggregate_cuda(Layer *layers, int batchNumber, int batchSize, int *V, int *E, float *edgeWeights, int layerNumber)
{
	int tid = threadIdx.x + blockIdx.x * blockDim.x;
	if (tid < batchSize)
	{
		int prevNumFeatures = layers[layerNumber - 1].num_features;
		int idx = batchNumber * batchSize + tid;
		// if (idx == 0)
		// printf("NUM FEATURES: %d and layernumber %d\n", layers[layerNumber - 1].num_features, layerNumber);
		for (int i = 0; i < prevNumFeatures; i++)
		{
			layers[layerNumber].aggregatedFeatures[idx * prevNumFeatures + i] = 0;
		}

		for (int j = V[idx]; j < V[idx + 1]; j++)
		{
			int neighbor = E[j];
			double weight = (double)edgeWeights[j];
			for (int i = 0; i < prevNumFeatures; i++)
			{
				layers[layerNumber].aggregatedFeatures[idx * prevNumFeatures + i] += layers[layerNumber - 1].postActivatedFeatures[neighbor * prevNumFeatures + i] * weight;
			}
			// if (idx == 102)
			// {
			// 	// Print the neighbour and the postact value of it for 0th feature
			// 	printf("Node %d: Neighbor = %d, PostAct = %f\n", idx, neighbor, layers[layerNumber - 1].postActivatedFeatures[neighbor * prevNumFeatures]);
			// 	// edge weight also
			// 	printf("Node %d: Neighbor = %d, EdgeWeight = %f\n", idx, neighbor, weight);
			// }			
		}
		// // if(epoch>1)
		// if(idx == 102)
		// printf("Node %d: Layer %d: aggregatedFeaturesnumber = %f\n", idx, layerNumber, layers[layerNumber].aggregatedFeatures[idx * layers[layerNumber - 1].num_features]);
	}
}
/*
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
*/

__global__ void NeuralNetworkOperation(Layer *layers, int batchNumber, int batchSize, int totalLayers, int layerNumber, int activationType)
{
	int tid = threadIdx.x + blockIdx.x * blockDim.x;
	if (tid < batchSize)
	{
		// printf("NN tid: %d\n", tid);

		int idx = batchNumber * batchSize + tid;
		int currNumFeatures = layers[layerNumber].num_features;
		int prevNumFeatures = layers[layerNumber - 1].num_features;
		if (layerNumber < totalLayers)
			for (int i = 0; i < currNumFeatures; i++)
			{
				double sum = 0.0f;

				for (int j = 0; j < prevNumFeatures; j++)
				{
					sum += layers[layerNumber].aggregatedFeatures[idx * prevNumFeatures + j] * layers[layerNumber].weights[j * currNumFeatures + i];
				}

				// if (idx == 102 && i == 0) {
				// 	for (int j = 0; j < prevNumFeatures; j++) {
				// 		printf("Node %d: index %d: Aggregated Features: %f\n", idx, j, layers[layerNumber].aggregatedFeatures[idx * prevNumFeatures + j]);
				// 		printf("Weight from feature %d to feature %d: %f\n", j, i, layers[layerNumber].weights[j * currNumFeatures + i]);
				// 	}
				// 	printf("Sum: %f\n", sum);
				// }

				layers[layerNumber].preActivatedFeatures[idx * currNumFeatures + i] = sum + layers[layerNumber].bias[i];
				if (layerNumber == 1 && idx == 0)
				{
					//// printf("preActivatedFeatures: %f\n", layers[layerNumber].preActivatedFeatures[idx * currNumFeatures + i]);
				}
				if (layerNumber == totalLayers - 1)
				{
					// softmax

					// softmax(layers[layerNumber].postActivatedFeatures[node], layers[layerNumber].num_features, layers[layerNumber].postActivatedFeatures[node]);
				}

				else if (activationType == 1)
				{
					layers[layerNumber].postActivatedFeatures[idx * currNumFeatures + i] = (layers[layerNumber].preActivatedFeatures[idx * currNumFeatures + i] > 0 ? layers[layerNumber].preActivatedFeatures[idx * currNumFeatures + i] : 0.0f);
				}
				else if (activationType == 2)
				{
					layers[layerNumber].postActivatedFeatures[idx * currNumFeatures + i] =
						tanh(layers[layerNumber].preActivatedFeatures[idx * currNumFeatures + i]);
				}
				// printf("Node %d: preActivatedFeatures = %f\n", idx, layers[layerNumber].preActivatedFeatures[idx * layers[layerNumber + 1].num_features + i]);

				// printf("Node %d: postActivatedFeatures = %f\n", idx, layers[layerNumber].postActivatedFeatures[idx * layers[layerNumber + 1].num_features + i]);
			}
	}
}

__global__ void computeGradientsOutputLayer(Layer *layers, int *y_true, int batchNumber, int batchSize, int num_classes, int num_layers)
{

	int tid = threadIdx.x + blockIdx.x * blockDim.x;
	if (tid < batchSize)
	{
		// printf("tid: %d\n", tid);

		int idx = batchNumber * batchSize + tid;
		int label = y_true[idx];

		double *y_pred = layers[num_layers - 1].preActivatedFeatures;
		// double *grad_output = globalCudaVar.layers[num_layers - 1].grad_pre_act_output;

		for (int j = 0; j < num_classes; ++j)
		{
			// grad_output[idx * num_classes + j] = y_pred[idx * num_classes + j] - (label == j ? 1.0f : 0.0f);
			layers[num_layers - 1].grad_pre_act_output[idx * num_classes + j] = y_pred[idx * num_classes + j] - (label == j ? 1.0f : 0.0f);
			// printf("grad_pre_act_output of layer %d entry %d: %f\n", num_layers - 1, idx * num_classes + j, layers[num_layers - 1].grad_pre_act_output[idx * num_classes + j]);
			// print y_pred
			// printf("y_pred for node %d for class %d: %f\n", idx, j, y_pred[idx * num_classes + j]);
		}
	}
}

__global__ void computeGradientsIntermediateLayer(Layer *layers, int batchNumber, int batchSize, int layer_num, int activation_type)
{
	int tid = threadIdx.x + blockIdx.x * blockDim.x;
	if (tid < batchSize)
	{
		// printf("tid: %d\n", tid);

		int current_num_features = layers[layer_num].num_features;
		int next_num_features = layers[layer_num + 1].num_features;

		int idx = batchNumber * batchSize + tid;
		for (int i = 0; i < current_num_features; ++i)
		{
			layers[layer_num].grad_pre_act_output[idx * current_num_features + i] = 0.0f;
			for (int j = 0; j < next_num_features; ++j)
			{
				layers[layer_num].grad_pre_act_output[idx * current_num_features + i] += layers[layer_num + 1].grad_pre_act_output[idx * next_num_features + j] * layers[layer_num + 1].weights[i * next_num_features + j];
			}
			// printf("grad_pre_act_output of layer %d entry %d: %f\n", layer_num, idx * current_num_features + i, layers[layer_num].grad_pre_act_output[idx * current_num_features + i]);
			if (activation_type == 1)
			{ // ReLU
				layers[layer_num].grad_pre_act_output[idx * current_num_features + i] *= (layers[layer_num].preActivatedFeatures[idx * current_num_features + i] > 0) ? 1 : 0;
			}
			else if (activation_type == 2)
			{ // Tanh
				layers[layer_num].grad_pre_act_output[idx * current_num_features + i] *= (1 - layers[layer_num].postActivatedFeatures[idx * current_num_features + i] * layers[layer_num].postActivatedFeatures[idx * current_num_features + i]);
			}
		}
	}
}

__global__ void computeWeightBiasGradients(Layer *layers, int layer_num, int num_nodes, float *weightNorm, float *biasNorm, int epoch)
{
	int featIdx = blockIdx.x * blockDim.x + threadIdx.x;
	int prev_features = layers[layer_num - 1].num_features;

	if (featIdx < prev_features)
	{
		int curr_num_features = layers[layer_num].num_features;
		for (int j = 0; j < curr_num_features; ++j)
		{
			double weight_grad_sum = 0.0;
			double bias_grad_sum = 0.0;

			// Accumulate gradient sums
			for (int nod = 0; nod < num_nodes; ++nod)
			{
				weight_grad_sum += layers[layer_num].aggregatedFeatures[nod * prev_features + featIdx] *
								   layers[layer_num].grad_pre_act_output[nod * curr_num_features + j];
				bias_grad_sum += layers[layer_num].grad_pre_act_output[nod * curr_num_features + j];
			}

			// Update gradient weights
			layers[layer_num].grad_weights[featIdx * curr_num_features + j] = weight_grad_sum;
// if(epoch ==3)
// printf("Aggregated Features of layer %d in epoch %d: %f\n", layer_num, epoch, layers[layer_num].grad_pre_act_output[featIdx * prev_features + j]);
			atomicAdd(weightNorm, (float)(weight_grad_sum * weight_grad_sum));
			// printf("Weight Norm: %f\n", *weightNorm);
			// Update bias gradient if featIdx == 0 to avoid overwrites
			if (featIdx == 0)
			{
				layers[layer_num].grad_bias[j] = bias_grad_sum;

				// Use atomicAdd to safely update biasNorm
				atomicAdd(biasNorm, (float)(bias_grad_sum * bias_grad_sum));
			}
		}
	}
}

__global__ void clipGradients(Layer *layers, int layer_num, float clip_value, float *weightNorm, float *biasNorm, int epochNumber)
{

	int tid = blockIdx.x * blockDim.x + threadIdx.x;
	double weightNorm1 = sqrtf(*weightNorm);
	double biasNorm1 = sqrtf(*biasNorm);
	// if(tid==0)
		// printf("WeightNorm of layer %d in epoch %d: %f\n", layer_num, epochNumber, weightNorm1);
	int num_input_features = layers[layer_num - 1].num_features;
	int num_output_features = layers[layer_num].num_features;
	if (tid < num_output_features)
	{
		for (int i = 0; i < num_input_features; i++)
		{
			int weight_idx = i * num_output_features + tid;
			if (weightNorm1 > clip_value)
			{
				// printf("Weight Norm: %f\n", weightNorm1);
				// printf("Pre update weight %d: %f\n", weight_idx, layers[layer_num].grad_weights[weight_idx]);
				layers[layer_num].grad_weights[weight_idx] *= clip_value / weightNorm1;
				// printf("Post update weight %d: %f\n", weight_idx, layers[layer_num].grad_weights[weight_idx]);
				
			}
		}
		if (biasNorm1 > clip_value)
		{
			layers[layer_num].grad_bias[tid] *= clip_value / biasNorm1;
		}
	}
}

__global__ void printprint(Layer *layers, int total_layers, int num_nodes, int epochNumber)
{
	int tid = threadIdx.x + blockIdx.x * blockDim.x;
	if (tid == 0)
	{
		// print all the values of all the layers
		for (int i = 1; i < total_layers; i++)
		{
			printf("\nLayer %d\n", i);
			for (int j = 0; j < layers[i].num_features * layers[i - 1].num_features; j++)
			{
				printf("Weight %d of layer %d in epoch %d: %f\n", j, i, epochNumber, layers[i].weights[j]);
			}
			// 	for(int j=0;j<layers[i].num_features;j++){
			// 		printf("Bias %d: %f\n", j, layers[i].bias[j]);
			// 	}
			// for (int j = 0; j < layers[i].num_features * num_nodes; j++)
			// {
			// 	printf("Node %d: preActivatedFeatures = %f\n", j, layers[i].preActivatedFeatures[j]);
			// 	printf("Node %d: postActivatedFeatures = %f\n", j, layers[i].postActivatedFeatures[j]);
			// }
			// Prints the grad weights
			// for (int j = 0; j < layers[i].num_features * layers[i - 1].num_features; j++)
			// {
			// 	printf("Grad Weight %d of layer %d in epoch %d: %f\n", j, i, epochNumber, layers[i].grad_weights[j]);
			// }
			// for (int j = 0; j < layers[i].num_features; j++)
			// {
			// 	printf("Grad Bias %d: %f\n", j, layers[i].grad_bias[j]);
			// }
		}
	}
}

__global__ void adam(Layer *layers, int num_layers, int batchNumber, int batchSize, double learning_rate, double beta1, double beta2, double epsilon, int t)
{
	int tid = threadIdx.x + blockIdx.x * blockDim.x;

	for (int i = 1; i < num_layers; i++)
	{
		int numfeats = layers[i].num_features;

		if (tid < numfeats)
		{
			int k = tid;

			double beta1_pow_t = pow(beta1, t); // Precompute beta1^t
			double beta2_pow_t = pow(beta2, t); // Precompute beta2^t
												// int num_features = layers[i].num_features;
			// Update biased first moment estimate for biases j =
			layers[i].m_biases[k] = beta1 * layers[i].m_biases[k] + (1 - beta1) * layers[i].grad_bias[k];
			// Update biased second raw moment estimate for biases
			layers[i].v_biases[k] = beta2 * layers[i].v_biases[k] + (1 - beta2) * layers[i].grad_bias[k] * layers[i].grad_bias[k];

			// Correct bias for the first and second moments (bias correction)
			double m_bias_hat = layers[i].m_biases[k] / (1 - beta1_pow_t);
			double v_bias_hat = layers[i].v_biases[k] / (1 - beta2_pow_t);

			// Update biases
			// printf("Pre updated bias %d: %f\n", k, layers[i].bias[k]);
			layers[i].bias[k] -= learning_rate * m_bias_hat / (sqrt(v_bias_hat) + epsilon);
			// printf("Post updated bias %d: %f\n", k, layers[i].bias[k]);
			for (int j = 0; j < layers[i - 1].num_features; j++)
			{
				int weight_idx = j * layers[i].num_features + k;

				// Update biased first moment estimate for weights
				layers[i].m_weights[weight_idx] = beta1 * layers[i].m_weights[weight_idx] + (1 - beta1) * layers[i].grad_weights[weight_idx];
				// Update biased second raw moment estimate for weights
				layers[i].v_weights[weight_idx] = beta2 * layers[i].v_weights[weight_idx] + (1 - beta2) * layers[i].grad_weights[weight_idx] * layers[i].grad_weights[weight_idx];

				// Correct bias for the first and second moments (bias correction)
				double m_weight_hat = layers[i].m_weights[weight_idx] / (1 - beta1_pow_t);
				double v_weight_hat = layers[i].v_weights[weight_idx] / (1 - beta2_pow_t);

				// Update weights
				// print pre update weights
				// printf(" Pre updated Weight %d: %f\n", weight_idx, layers[i].weights[weight_idx]);
				layers[i].weights[weight_idx] -= learning_rate * (m_weight_hat / (sqrt(v_weight_hat) + epsilon) );

				// print the weights
				//  printf("Post updated Weight %d: %f\n", weight_idx, layers[i].weights[weight_idx]);
			}
		}
	}
}

// __global__ void calculateAccuracy(Layer *layers, int *ground_truth, int *correct_predictions, int num_classes, int batchNumber, int batchSize)
__global__ void calculateAccuracy(Layer *layers, int *ground_truth, int *correct_predictions, int num_classes, int num_nodes, int num_layers)
{
	int idx = threadIdx.x + blockIdx.x * blockDim.x;

	// printf("JEIIJ\n");
	if (idx < num_nodes)
	{
		double *predictions = layers[num_layers - 1].preActivatedFeatures;
		// Find the index of the maximum predicted class (argmax)
		int predicted_class = 0;
		double max_value = predictions[idx * num_classes];

		for (int i = 0; i < num_classes; ++i)
		{
			if (predictions[idx * num_classes + i] > max_value)
			{
				max_value = predictions[idx * num_classes + i];
				// printf("max_value: %f\n", max_value);
				predicted_class = i;
			}
		}
		// printf("max_value: %f\n", max_value);

		// Compare with the ground truth
		if (predicted_class == ground_truth[idx])
		{
			atomicAdd(correct_predictions, 1); // Increment the counter for correct predictions
		}
	}
}

void forwardPass_cuda(GNN &gnn, int layerNumber, int batchNumber, int batchSize, int activationType)
{
	if (layerNumber == 0)
	{
		return;
	}
	int numNodesInBatch = batchSize;
	int threadsPerBlock = 1024;
	int blocksPerGrid = (numNodesInBatch + threadsPerBlock - 1) / threadsPerBlock;

	Layer *layers = globalCudaVar.layers;

	// printf("aa blocksPerGrid: %d\n", blocksPerGrid);
	// printf("aa threadsPerBlock: %d\n", threadsPerBlock);
	// printf("aa Total threads: %d\n", blocksPerGrid * threadsPerBlock);

	// GCN_aggregate_cuda<<<blocksPerGrid, threadsPerBlock, 0, stream1>>>(layers, numNodes, gnn.getGraph().getNodesptr(), gnn.getGraph().getEdgesList(), gnn.getGraph().getEdgeLen(), layerNumber, node, activationType);
	GCN_aggregate_cuda<<<blocksPerGrid, threadsPerBlock>>>(layers, batchNumber, batchSize, globalCudaVar.d_nodesPtr, globalCudaVar.d_edgesList, globalCudaVar.d_edgeWeights, layerNumber);
	cudaDeviceSynchronize();

	cudaError_t err1 = cudaGetLastError();
	if (err1 != cudaSuccess)
	{
		printf("CUDA aaError1: %s\n", cudaGetErrorString(err1));
	}

	NeuralNetworkOperation<<<blocksPerGrid, threadsPerBlock>>>(layers, batchNumber, batchSize, 3, layerNumber, activationType);
	cudaDeviceSynchronize();

	cudaError_t err = cudaGetLastError();
	if (err != cudaSuccess)
	{
		printf("CUDA ffError1: %s\n", cudaGetErrorString(err));
	}
}

void backPropagation_cuda(GNN &gnn, int layerNumber, int totalLayers, int batchNumber, int batchSize, int num_features, int epochNumber, int activationType)
{
	// printf("Backpropagation Started\n");
	if (layerNumber == 0)
	{
		return;
	}
	// double *d_y_pred, *d_grad_output, *d_weights, *d_preActivatedFeatures, *d_aggregatedFeatures, *d_bias;
	if (layerNumber == totalLayers - 1)
	{
		int threads = 1024;
		int blocks = (batchSize + threads - 1) / threads;
		computeGradientsOutputLayer<<<blocks, threads>>>(globalCudaVar.layers, globalCudaVar.d_y_true, batchNumber, batchSize, gnn.numClasses(), totalLayers);
		cudaDeviceSynchronize();
	}
	else
	{
		int threads = 1024;
		int blocks = (batchSize + threads - 1) / threads;
		computeGradientsIntermediateLayer<<<blocks, threads>>>(globalCudaVar.layers, batchNumber, batchSize, layerNumber, activationType);
		cudaDeviceSynchronize();
	}
	cudaError_t err1 = cudaGetLastError();
	if (err1 != cudaSuccess)
	{
		printf("CUDA bp1Error: %s\n", cudaGetErrorString(err1));
	}

	int threads = 1024;
	int blocks = (num_features + threads - 1) / threads;

	float *d_weightNorm, *d_biasNorm;
	cudaMalloc(&d_weightNorm, sizeof(float));
	cudaMalloc(&d_biasNorm, sizeof(float));
	cudaMemset(d_weightNorm, 0, sizeof(float));
	cudaMemset(d_biasNorm, 0, sizeof(float));

	computeWeightBiasGradients<<<blocks, threads>>>(globalCudaVar.layers, layerNumber, gnn.getGraph().num_nodes(), d_weightNorm, d_biasNorm, epochNumber);
	cudaDeviceSynchronize();

	clipGradients<<<blocks, threads>>>(globalCudaVar.layers, layerNumber, GRADIENT_NORM_CLIP_VALUE, d_weightNorm, d_biasNorm, epochNumber);
	cudaDeviceSynchronize();

	cudaError_t err = cudaGetLastError();
	if (err != cudaSuccess)
	{
		printf("CUDA bpError: %s\n", cudaGetErrorString(err));
	}
}

void adamOptimizer_cuda(vector<int> numFeaturesPerLayer, int num_layers, int batchNumber, int batchSize, double learning_rate, double beta1, double beta2, double epsilon, int epochNumber)
{
	int t = epochNumber * (batchSize + 1);
	int threads = 1024;
	// int blocks = (batchSize + threads - 1) / threads;
	// Total number of threads is equal to the maximum of number of features among every layer
	int max_features = 0;
	int cpu_layers = num_layers;
	for (int i = 0; i < cpu_layers; i++)
	{
		if (numFeaturesPerLayer[i] > max_features)
		{
			max_features = numFeaturesPerLayer[i];
		}
	}

	int blocks = (max_features + threads - 1) / threads;

	adam<<<blocks, threads>>>(globalCudaVar.layers, num_layers, batchNumber, batchSize, learning_rate, beta1, beta2, epsilon, t);
	cudaDeviceSynchronize();
	cudaError_t err = cudaGetLastError();
	if (err != cudaSuccess)
	{
		printf("CUDA opError: %s\n", cudaGetErrorString(err));
	}
}

void predict_cuda(Layer *d_layers, int num_classes, int num_nodes, int totalLayers)
{
	int *d_correct_predictions, correct_predictions = 0;
	cudaMalloc(&d_correct_predictions, sizeof(int));
	cudaMemcpy(d_correct_predictions, &correct_predictions, sizeof(int), cudaMemcpyHostToDevice);
	cudaError_t err = cudaGetLastError();
	if (err != cudaSuccess)
	{
		printf("CUDA t1Error: %s\n", cudaGetErrorString(err));
	}
	int threadsPerBlock = 1024;
	int blocksPerGrid = (num_nodes + threadsPerBlock - 1) / threadsPerBlock;
	// printf("Test Accuracy\n");
	// Launch the accuracy calculation kernel
	// printf("hjfjhv\n");
	calculateAccuracy<<<blocksPerGrid, threadsPerBlock>>>(globalCudaVar.layers, globalCudaVar.d_y_true, d_correct_predictions, num_classes, num_nodes, totalLayers);
	cudaDeviceSynchronize();

	cudaError_t err1 = cudaGetLastError();
	if (err1 != cudaSuccess)
	{
		printf("CUDA ttError: %s\n", cudaGetErrorString(err1));
	}
	cudaMemcpy(&correct_predictions, d_correct_predictions, sizeof(int), cudaMemcpyDeviceToHost);

	// Calculate and print the accuracy
	double accuracy = (double)correct_predictions / (double)num_nodes;
	printf("Test Accuracy: %.2f%%\n", accuracy * 100);
	fflush(stdout);

	cudaFree(d_correct_predictions);
}

int main()
{
	// vector<int> numFeaturesPerLayer = {500, 16, 3}; //pubmed
	// vector<int> numFeaturesPerLayer = {4973, 40, 17}; //wiki
	vector<int> numFeaturesPerLayer = {500, 16, 7}; // flickr
	// vector<int> numFeaturesPerLayer = {16, 8, 2};//sample graph
	// vector<int> numFeaturesPerLayer = {745, 16, 8}; // amazon
	const char *transformation = "xaviers";
	// graph G("/home/anubhav/new_q/newww_tha/Starplat-GNN/graphcode/sample_graphs/Wiki/wiki_edgelist.txt");
	// graph G("/home/anubhav/new_q/newww_tha/Starplat-GNN/graphcode/sample_graphs/Amazon/amazon_edgelist.txt");
	graph G("/home/anubhav/new_q/newww_tha/Starplat-GNN/graphcode/sample_graphs/flickr/flickr_edgelist.txt");

	// graph G("/home/anubhav/new_q/newww_tha/Starplat-GNN/graphcode/sample_graphs/Pubmed/pubmed_edgelist.txt");
	// graph G("/home/anubhav/new_q/newww_tha/Starplat-GNN/graphcode/sample_graphs/sample_graph/sample_graph.txt");
	G.parseGraph();
	// GNN gnn(G, "/home/anubhav/new_q/newww_tha/Starplat-GNN/graphcode/sample_graphs/Amazon/amazon_features.txt", "/home/anubhav/new_q/newww_tha/Starplat-GNN/graphcode/sample_graphs/Amazon/amazon_labels.txt");
	GNN gnn(G,"/home/anubhav/new_q/newww_tha/Starplat-GNN/graphcode/sample_graphs/flickr/flickr_features.txt","/home/anubhav/new_q/newww_tha/Starplat-GNN/graphcode/sample_graphs/flickr/flickr_labels.txt" );

	// GNN gnn(G,"/home/anubhav/new_q/newww_tha/Starplat-GNN/graphcode/sample_graphs/Wiki/wiki_features.txt","/home/anubhav/new_q/newww_tha/Starplat-GNN/graphcode/sample_graphs/Wiki/wiki_labels.txt" );
	// GNN gnn(G, "/home/anubhav/new_q/newww_tha/Starplat-GNN/graphcode/sample_graphs/Pubmed/pubmed_features.txt", "/home/anubhav/new_q/newww_tha/Starplat-GNN/graphcode/sample_graphs/Pubmed/pubmed_labels.txt");
	// GNN gnn(G, "/home/anubhav/new_q/newww_tha/Starplat-GNN/graphcode/sample_graphs/sample_graph/sample_graph_feat.txt", "/home/anubhav/new_q/newww_tha/Starplat-GNN/graphcode/sample_graphs/sample_graph/sample_graph_labels.txt");
	gnn.gcnPreprocessing();
	cout << "preprosessing done" << endl;
	printf("num_layers_cpu: %d\n", gnn.getLayers().size());

	int num_layers = numFeaturesPerLayer.size();

	Layer *d_layers;
	cudaMalloc(&d_layers, num_layers * sizeof(Layer));

	Layer *h_layers = new Layer[num_layers];
	for (int i = 0; i < num_layers; i++)
	{
		h_layers[i] = Layer();
		cudaMalloc(&h_layers[i].weights, numFeaturesPerLayer[(i > 0 ? i - 1 : i)] * numFeaturesPerLayer[i] * sizeof(double));
		cudaMalloc(&h_layers[i].bias, numFeaturesPerLayer[i] * sizeof(double));
		cudaMalloc(&h_layers[i].grad_weights, numFeaturesPerLayer[(i > 0 ? i - 1 : i)] * numFeaturesPerLayer[i] * sizeof(double));
		cudaMalloc(&h_layers[i].grad_bias, numFeaturesPerLayer[i] * sizeof(double));
		cudaMalloc(&h_layers[i].m_weights, numFeaturesPerLayer[(i > 0 ? i - 1 : i)] * numFeaturesPerLayer[i] * sizeof(double));
		cudaMalloc(&h_layers[i].m_biases, numFeaturesPerLayer[i] * sizeof(double));
		cudaMalloc(&h_layers[i].v_weights, numFeaturesPerLayer[(i > 0 ? i - 1 : i)] * numFeaturesPerLayer[i] * sizeof(double));
		cudaMalloc(&h_layers[i].v_biases, numFeaturesPerLayer[i] * sizeof(double));

		cudaMalloc(&h_layers[i].aggregatedFeatures, gnn.getGraph().num_nodes() * numFeaturesPerLayer[i > 0 ? i - 1 : i] * sizeof(double));
		cudaMalloc(&h_layers[i].preActivatedFeatures, gnn.getGraph().num_nodes() * numFeaturesPerLayer[i] * sizeof(double));
		cudaMalloc(&h_layers[i].postActivatedFeatures, gnn.getGraph().num_nodes() * numFeaturesPerLayer[i] * sizeof(double));
		cudaMalloc(&h_layers[i].grad_pre_act_output, gnn.getGraph().num_nodes() * numFeaturesPerLayer[i] * sizeof(double));

		cudaMemcpy(&d_layers[i], &h_layers[i], sizeof(Layer), cudaMemcpyHostToDevice);
	}
	vector<vector<double>> feats = gnn.getFeatures();
	// convert it to a 1D array
	// vector<double> flat_feats;
	// for (int i = 0; i < feats.size(); i++)
	// {
	// 	for (int j = 0; j < feats[i].size(); j++)
	// 	{
	// 		flat_feats.push_back(feats[i][j]);
	// 	}
	// }

	// In host
	double *flat_feats = new double[gnn.getGraph().num_nodes() * feats[0].size()];
	for (int i = 0; i < gnn.getGraph().num_nodes(); i++)
	{
		for (int j = 0; j < feats[0].size(); j++)
		{
			flat_feats[i * feats[0].size() + j] = feats[i][j];
		}
	}

	double *d_features;
	cudaMalloc(&d_features, gnn.getGraph().num_nodes() * feats[0].size() * sizeof(double));
	// .data of vector of vector is a 1D array
	cudaMemcpy(d_features, flat_feats, gnn.getGraph().num_nodes() * feats[0].size() * sizeof(double), cudaMemcpyHostToDevice);

	initializeLayers_cuda(gnn, d_layers, numFeaturesPerLayer, d_features, gnn.getGraph().num_nodes(), transformation);
	cudaDeviceSynchronize();

	// printprint<<<1, 1>>>(d_layers, numFeaturesPerLayer.size(), gnn.getGraph().num_nodes(),1);
	// cudaDeviceSynchronize();

	double *features = new double[gnn.getGraph().num_nodes() * feats[0].size()];
	cudaMemcpy(features, d_features, gnn.getGraph().num_nodes() * feats[0].size() * sizeof(double), cudaMemcpyDeviceToHost);

	// printf("\n\ndone\n\n");
	// traverse all the layers for forward porpagation

	int num_nodes = gnn.getGraph().num_nodes();
	// int num_nodes = 8000;

		// printprint<<<1, 1>>>(globalCudaVar.layers, numFeaturesPerLayer.size(), gnn.getGraph().num_nodes(), 1);
		// cudaDeviceSynchronize();
    // Create CUDA events
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    // Record the start event
    cudaEventRecord(start);
	for (int epoch = 1; epoch < 50; epoch++)
	{
		for (int i = 0; i < 3; i++)
		{
			forwardPass_cuda(gnn, i, 0, num_nodes, 1);
		}
		// printf("\n\n");
		// if(epoch == 3)
		// exit(0);

		for (int i = 2; i >= 0; i--)
		{
			backPropagation_cuda(gnn, i, numFeaturesPerLayer.size(), 0, num_nodes, numFeaturesPerLayer[i], epoch, 1);
		}
		// printf("Bp over\n");

		adamOptimizer_cuda(numFeaturesPerLayer, numFeaturesPerLayer.size(), 0, gnn.getGraph().num_nodes(), 0.001, 0.9, 0.999, 1e-8, epoch);
		// exit(0);
		// printf("Optimization done\n");
		predict_cuda(d_layers, gnn.numClasses(), num_nodes, numFeaturesPerLayer.size());
		// printf("Accuracy calculated\n");
	}
	   cudaEventRecord(stop);

    // Wait for the stop event to complete
    cudaEventSynchronize(stop);

    // Calculate the elapsed time
    float milliseconds = 0;
    cudaEventElapsedTime(&milliseconds, start, stop);

    std::cout << "Time elapsed: " << milliseconds << " ms" << std::endl;

    // Clean up
    cudaEventDestroy(start);
    cudaEventDestroy(stop);

	cudaError_t err = cudaGetLastError();
	if (err != cudaSuccess)
	{
		printf("CUDA ffError: %s\n", cudaGetErrorString(err));
	}
	//  double *features = new double[num_nodes * feats[0].size()];
	//  cudaMemcpy(features, d_features, num_nodes * feats[0].size() * sizeof(double), cudaMemcpyDeviceToHost);
	//  for (int i = 0; i < feats[0].size(); i++)
	//  {
	//  	cout << features[4 * feats[0].size() + i] << " ";
	//  }

	// print features of node 4
	// double *featuress = new double[gnn.getGraph().num_nodes() * feats[0].size()];
	// cudaMemcpy(featuress, h_layers[1].preActivatedFeatures, gnn.getGraph().num_nodes() * feats[0].size() * sizeof(double), cudaMemcpyDeviceToHost);

	return 0;
}
