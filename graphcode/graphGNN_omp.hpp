#include "graph.hpp"



void XaviersInit(float **weights, int num_neurons, int num_features)
{
  //xaiver's initialization
  double xavier = sqrt(6.0 / (num_neurons + num_features));
  std::random_device rd; // Obtain a random number from hardware
  std::mt19937 gen(rd()); // Seed the generator
  std::uniform_real_distribution<> dist(0.0, xavier); 
  for (int i = 0; i < num_neurons; i++)
  {
    for (int j = 0; j < num_features; j++)
    {
      weights[i][j] = dist(gen);
    }
  }
}

void normalDistributionInit(float **weights, int num_neurons, int num_features)
{
    double mean = 0.0;
    double std = 0.02;
    std::random_device rd; // Obtain a random number from hardware
    std::mt19937 gen(rd()); // Seed the generator
    std::normal_distribution<> dist(mean, std);
    for (int i = 0; i < num_neurons; i++)
    {
      for (int j = 0; j < num_features; j++)
      {
        weights[i][j] = dist(gen);
      }
    }
}
void uniformDistributionInit(float **weights, int num_neurons, int num_features)
{
  //uniform distribution
  double min = -0.5;
  double max = 0.5;
  std::random_device rd; // Obtain a random number from hardware
  std::mt19937 gen(rd()); // Seed the generator
  std::uniform_real_distribution<> dist(min, max); 
  for (int i = 0; i < num_neurons; i++)
  {
    for (int j = 0; j < num_features; j++)
    {
      weights[i][j] = dist(gen);
    }
  }

}

void gcn_preprocessing_omp(GNN &gnn)
{
  // Symmetric notmalization of the adjacency matrix (present in CSR form)
  // Add self loops to the adjacency matrix

  graph &g = gnn.getGraph();
  // change weight of all edges to 1
  int num_edges = g.num_edges();
#pragma omp parallel for
  for (int nod = 0; nod < g.num_nodes(); nod++)
  {
    for (auto &edge : g.getNeighbors(nod))
    {
      g.changeWeight(edge.source, edge.destination, 1);
    }
  }

  int num_nodes = g.num_nodes();
  for (int i = 0; i < num_nodes; i++)
  {
    g.addEdge(i, i, 1);
  }

  // calculate degree matrix
  std::vector<float> degree(num_nodes, 0);
#pragma omp parallel for
  for (int nod = 0; nod < num_nodes; nod++)
  {
    for (auto edge : g.getNeighbors(nod))
    {
      degree[nod] += edge.weight;
    }
  }

  // calculate inverse of degree matrix
  std::vector<float> inv_degree(num_nodes, 0);
#pragma omp parallel for
  for (int nod = 0; nod < num_nodes; nod++)
  {
    inv_degree[nod] = 1 / sqrt(degree[nod]);
  }

  // calculate the normalized adjacency matrix
#pragma omp parallel for
  for (int nod = 0; nod < num_nodes; nod++)
  {
    for (auto &edge : g.getNeighbors(nod))
    {
      g.changeWeight(edge.source, edge.destination, edge.weight * inv_degree[edge.source] * inv_degree[edge.destination]);
    }
  }
}

float relu(float x)
{
  return x > 0 ? x : 0;
}

float derivative_relu(float x)
{
  return x > 0 ? 1 : 0;
}

float softmax(float *x, int size)
{
  float sum = 0;
  for (int i = 0; i < size; i++)
  {
    sum += exp(x[i]);
  }
  for (int i = 0; i < size; i++)
  {
    x[i] = exp(x[i]) / sum;
  }
}
float softmax_derivative(float *x, int size)
{
  for (int i = 0; i < size; i++)
  {
    x[i] = x[i] * (1 - x[i]);
  }
}


void initializeLayers_omp(GNN &gnn, std::vector<int> neuronsPerLayer, char *initType){
  std::vector<Layer> layers(neuronsPerLayer.size());

    for(int i=0; i<neuronsPerLayer.size(); i++){
      layers[i].num_features = neuronsPerLayer[i];
      layers[i].weights = new float*[neuronsPerLayer[i]];
      for(int j=0; j<neuronsPerLayer[i]; j++){
        layers[i].weights[j] = new float[neuronsPerLayer[i+1]];
      }
      layers[i].bias = new float[neuronsPerLayer[i+1]];
      layers[i].output = new float[neuronsPerLayer[i+1]];
      layers[i].input = new float[neuronsPerLayer[i]];
      layers[i].grad_input = new float[neuronsPerLayer[i]];
      layers[i].grad_weights = new float*[neuronsPerLayer[i]];
      for(int j=0; j<neuronsPerLayer[i]; j++){
        layers[i].grad_weights[j] = new float[neuronsPerLayer[i+1]];
      }
      layers[i].grad_bias = new float[neuronsPerLayer[i+1]];
      layers[i].grad_output = new float[neuronsPerLayer[i+1]];
    }
    if(strcmp(initType, "xaviers")==0){
      for(int i=0; i<neuronsPerLayer.size()-1; i++){
        XaviersInit(layers[i].weights, neuronsPerLayer[i], neuronsPerLayer[i+1]);
      }
    }
    else if(strcmp(initType, "normal")==0){
      for(int i=0; i<neuronsPerLayer.size()-1; i++){
        normalDistributionInit(layers[i].weights, neuronsPerLayer[i], neuronsPerLayer[i+1]);
      }
    }
    else if(strcmp(initType, "uniform")==0){
      for(int i=0; i<neuronsPerLayer.size()-1; i++){
        uniformDistributionInit(layers[i].weights, neuronsPerLayer[i], neuronsPerLayer[i+1]);
      }
    }


 
for(int i=0; i<neuronsPerLayer.size(); i++){
    std::cout<<"Layer "<<layers[i].num_features<<" initialized"<<std::endl;
  }

} 
void getLayers_omp(GNN &gnn){
  return gnn.getLayers();
}
void calculategradoutput_omp(GNN &gnn, int layer){
  std::vector<Layer> layers = gnn.getLayers();
  for(int i=0; i<layers[layer].num_features; i++){
    layers[layer].grad_output[i] = layers[layer].output[i] - gnn.getLabels()[i];
  }
}

void calculategradinput_omp(GNN &gnn, int layer){
  std::vector<Layer> layers = gnn.getLayers();
  for(int nod=0; nod<gnn.getGraph().num_nodes(); nod++){
    for(auto edge: gnn.getGraph().getNeighbors(nod)){
      for(int i=0; i<layers[layer].num_features; i++){
        layers[layer].grad_input[nod][i] += layers[layer].grad_output[edge.destination][i] * layers[layer+1].weights[i][edge.destination];
      }
    }
  }
}



void getWeights_omp(GNN &gnn, int layer){
  std::vector<Layer> layers = gnn.getLayers();
  return layers[layer].weights;
}

void forwardPass_omp(GNN &gnn, Layer layers)
{
  graph &g = gnn.getGraph();
  int num_nodes = g.num_nodes();
  int num_edges = g.num_edges();
  // forward pass for graph neural networks
  #pragma omp parallel for schedule(dynamic)
  for (int nod = 0; nod < num_nodes; nod++)
  {
    for (int i = 0; i < layers.num_features; i++)
    {
      layers.input[i] = 0;
      for (auto edge : g.getNeighbors(nod))
      {
        layers.input[i] += layers.weights[edge.destination][i] * g.getWeight(edge.source, edge.destination);
      }
      layers.output[i] = relu(layers.input[i] + layers.bias[i]);
    }   
  }
}




/*
void backPropagation(GNN &gnn, int layer)
{
  graph &g = gnn.getGraph();
  std::vector<std::vector<float>> new_features = gnn.getFeatures();
  std::vector<std::vector<float>> old_features = gnn.getFeatures();
  std::vector<std::vector<float>> weights = gnn.getWeights(layer);
  std::vector<std::vector<float>> biases = gnn.getBiases(layer);
  std::vector<std::vector<float>> grad_weights = gnn.getGradWeights(layer);
  std::vector<std::vector<float>> grad_biases = gnn.getGradBiases(layer);
  std::vector<std::vector<float>> grad_output = gnn.getGradOutput(layer);
  std::vector<std::vector<float>> grad_input = gnn.getGradInput(layer);

  // backpropagation for graph neural networks
  #pragma omp parallel for schedule(dynamic)
  for (int nod = 0; nod < g.num_nodes(); nod++)
  {
    for (auto edge : g.getNeighbors(nod))
    {
      for (int i = 0; i < gnn.getFeatureSize(); i++)
      {
        grad_weights[edge.destination][i] += grad_output[nod][i] * old_features[edge.destination][i];
        grad_biases[edge.destination] += grad_output[nod][i];
        grad_input[nod][i] += grad_output[nod][i] * weights[edge.destination][i];
      }
    }
  }
}
void adam(GNN &gnn, int layer, float lr, float beta1, float beta2, float epsilon)
{
  std::vector<std::vector<float>> weights = gnn.getWeights(layer);
  std::vector<std::vector<float>> biases = gnn.getBiases(layer);
  std::vector<std::vector<float>> grad_weights = gnn.getGradWeights(layer);
  std::vector<std::vector<float>> grad_biases = gnn.getGradBiases(layer);
  std::vector<std::vector<float>> m_weights = gnn.getMWeights(layer);
  std::vector<std::vector<float>> m_biases = gnn.getMBiases(layer);
  std::vector<std::vector<float>> v_weights = gnn.getVWeights(layer);
  std::vector<std::vector<float>> v_biases = gnn.getVBiases(layer);
  int t = gnn.getT(layer);

  // adam optimizer
  #pragma omp parallel for schedule(dynamic)
  for (int i = 0; i < weights.size(); i++)
  {
    for (int j = 0; j < weights[i].size(); j++)
    {
      m_weights[i][j] = beta1 * m_weights[i][j] + (1 - beta1) * grad_weights[i][j];
      v_weights[i][j] = beta2 * v_weights[i][j] + (1 - beta2) * grad_weights[i][j] * grad_weights[i][j];
      float m_hat = m_weights[i][j] / (1 - pow(beta1, t));
      float v_hat = v_weights[i][j] / (1 - pow(beta2, t));
      weights[i][j] -= lr * m_hat / (sqrt(v_hat) + epsilon);
    }
  }

  #pragma omp parallel for schedule(dynamic)
  for (int i = 0; i < biases.size(); i++)
  {
    m_biases[i] = beta1 * m_biases[i] + (1 - beta1) * grad_biases[i];
    v_biases[i] = beta2 * v_biases[i] + (1 - beta2) * grad_biases[i] * grad_biases[i];
    float m_hat = m_biases[i] / (1 - pow(beta1, t));
    float v_hat = v_biases[i] / (1 - pow(beta2, t));
    biases[i] -= lr * m_hat / (sqrt(v_hat) + epsilon);
  }
  gnn.setT(layer, t + 1);
}*/
