#include "graph.hpp"

void xaviersInit_omp(float **weights, int num_neurons_current, int num_features_next)
{
  // xaiver's initialization
  double xavier = sqrt(6.0 / (num_neurons_current + num_features_next));
  std::random_device rd; // Obtain a random number from hardware
  std::mt19937 gen(42);  // Seed the generator
  std::uniform_real_distribution<> dist(0.0, xavier);
  std::cout << "xavier distribution with input neurons : " << num_neurons_current << " and output neurons : " << num_features_next << std::endl;
  for (int i = 0; i < num_neurons_current; i++)
  {
    for (int j = 0; j < num_features_next; j++)
    {
      weights[i][j] = dist(gen);
      // std::cout << weights[i][j] << " ";
    }
    // std::cout << std::endl;
  }
}

void normalDistributionInit_omp(float **weights, int num_neurons_current, int num_features_next)
{
  double mean = 0.0;
  double std = 0.02;
  std::random_device rd; // Obtain a random number from hardware
  std::mt19937 gen(42);  // Seed the generator
  std::normal_distribution<> dist(mean, std);
  // std::cout << "normal distribution with input neurons : " << num_neurons_current << " and output neurons : " << num_features_next << std::endl;
  // for (int i = 0; i < num_neurons_current; i++)
  // {
  //   for (int j = 0; j < num_features_next; j++)
  //   {
  //     weights[i][j] = dist(gen);
  //   }
  // }
}
void uniformDistributionInit_omp(float **weights, int num_neurons_current, int num_features_next)
{
  // uniform distribution
  double min = -0.5;
  double max = 0.5;
  std::random_device rd; // Obtain a random number from hardware
  std::mt19937 gen(42);  // Seed the generator
  std::uniform_real_distribution<> dist(min, max);
  std::cout << "uniform distributionx  with input neurons : " << num_neurons_current << " and output neurons : " << num_features_next << std::endl;
  for (int i = 0; i < num_neurons_current; i++)
  {
    for (int j = 0; j < num_features_next; j++)
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

void initializeLayers_omp(GNN &gnn, std::vector<int32_t> neuronsPerHiddenLayer, char *initType)
{
  std::vector<int32_t> neuronsPerLayer;
  neuronsPerLayer.push_back(gnn.numFeatures());
  std::cout << "Neurons initially" << gnn.numFeatures() << std::endl;
  for (int i = 0; i < neuronsPerHiddenLayer.size(); i++)
  {
    neuronsPerLayer.push_back(neuronsPerHiddenLayer[i]);
  }
  neuronsPerLayer.push_back(gnn.numClasses());
  std::vector<layer> &layers = gnn.getLayers();
  layers.resize(neuronsPerLayer.size());

  for (int i = 0; i < neuronsPerLayer.size() - 1; i++)
  {
    layers[i].num_features = neuronsPerLayer[i];
    layers[i].weights = new float *[neuronsPerLayer[i]];

    for (int x = 0; x < layers[i].num_features; x++)
    {
      layers[i].weights[x] = new float[neuronsPerLayer[i + 1]];
    }

    layers[i].inputFeatures = new float *[gnn.getGraph().num_nodes()];
    layers[i].outputFeatures = new float *[gnn.getGraph().num_nodes()];
    for (int j = 0; j < gnn.getGraph().num_nodes(); j++)
    {
      layers[i].inputFeatures[j] = new float[neuronsPerLayer[i]];
      if (i < neuronsPerLayer.size() - 1)
        layers[i].outputFeatures[j] = new float[neuronsPerLayer[i + 1]];
    }

    layers[i].bias = new float[neuronsPerLayer[i + 1]];
    layers[i].grad_input = new float[neuronsPerLayer[i]];
    layers[i].grad_weights = new float *[neuronsPerLayer[i]];
    for (int j = 0; j < neuronsPerLayer[i]; j++)
    {
      layers[i].grad_weights[j] = new float[neuronsPerLayer[i + 1]];
    }
    layers[i].grad_bias = new float[neuronsPerLayer[i + 1]];
    layers[i].grad_output = new float[neuronsPerLayer[i + 1]];

    // std::cout << "layer with size " << layers[i].num_features << " initialized" << std::endl;
  }
  layers[neuronsPerLayer.size() - 1].num_features = neuronsPerLayer[neuronsPerLayer.size() - 1];
  layers[neuronsPerLayer.size() - 1].bias = new float[neuronsPerLayer[neuronsPerLayer.size() - 1]];
  layers[neuronsPerLayer.size() - 1].grad_bias = new float[neuronsPerLayer[neuronsPerLayer.size() - 1]];
  layers[neuronsPerLayer.size() - 1].grad_output = new float[neuronsPerLayer[neuronsPerLayer.size() - 1]];
  std::vector<std::vector<float>> features = gnn.getFeatures();

  for (int j = 0; j < gnn.getGraph().num_nodes(); j++)
  {
    for (int i = 0; i < gnn.numFeatures(); i++)
    {
      layers[0].inputFeatures[j][i] = features[j][i];
      // std::cout << layers[0].inputFeatures[j][i] << " ";
    }
    // std::cout << std::endl;
  }

  if (strcmp(initType, "xaviers") == 0)
  {
    for (int i = 0; i < neuronsPerLayer.size() - 1; i++)
    {
      xaviersInit_omp(layers[i].weights, neuronsPerLayer[i], neuronsPerLayer[i + 1]);
    }
  }
  else if (strcmp(initType, "normal") == 0)
  {
    for (int i = 0; i < neuronsPerLayer.size() - 1; i++)
    {
      normalDistributionInit_omp(layers[i].weights, neuronsPerLayer[i], neuronsPerLayer[i + 1]);
    }
  }
  else if (strcmp(initType, "uniform") == 0)
  {
    for (int i = 0; i < neuronsPerLayer.size() - 1; i++)
    {
      uniformDistributionInit_omp(layers[i].weights, neuronsPerLayer[i], neuronsPerLayer[i + 1]);
    }
  }
  else
  {
    std::cout << "Invalid initialization type" << std::endl;
  }
}

// void calculategradoutput_omp(GNN &gnn, int layer)
// {
//   std::vector<layer> &layers = gnn.getLayers();
//   for (int i = 0; i < layers[layer].num_features; i++)
//   {
//     layers[layer].grad_output[i] = layers[layer].outputFeatures[i] - gnn.getLabels()[i];
//   }
// }
void getWeights_omp(GNN &gnn, int layerr)
{
  std::vector<layer> &layers = gnn.getLayers();
  for (int i = 0; i < layers[layerr].num_features; i++)
  {
    for (int j = 0; j < layers[layerr + 1].num_features; j++)
    {
      layers[layerr].weights[i][j] = layers[layerr].weights[i][j];
    }
  }
}

// void calculategradinput_omp(GNN &gnn, int layer)
// {
//   std::vector<layer> layers = gnn.getLayers();
//   for (int nod = 0; nod < gnn.getGraph().num_nodes(); nod++)
//   {
//     for (auto edge : gnn.getGraph().getNeighbors(nod))
//     {
//       for (int i = 0; i < layers[layer].num_features; i++)
//       {
//         layers[layer].grad_input[nod][i] += layers[layer].grad_output[edge.destination][i] * layers[layer + 1].weights[i][edge.destination];
//       }
//     }
//   }
// }

void aggregate_omp(GNN &gnn, int node, int layerNumber)
{
  graph &g = gnn.getGraph();
  std::vector<layer> &layers = gnn.getLayers();

  for (auto edge : g.getNeighbors(node))
  {
    // int c = 0;
    for (int i = 0; i < layers[layerNumber].num_features; i++)
    {

      layers[layerNumber].inputFeatures[node][i] += layers[layerNumber].inputFeatures[edge.destination][i] * edge.weight;
      std::cout << layers[0].inputFeatures[node][i] << " ";
      // c++;
    }
    std::cout << std::endl;
    //   std::cout<<std::endl;
    //   std::cout<< c <<std::endl;
  }
}

void forwardPass_omp(GNN &gnn, int node, int layerNumber) //, char *aggregationType)
{
  std::vector<layer> &layers = gnn.getLayers();
  graph &g = gnn.getGraph();
  aggregate_omp(gnn, node, layerNumber);

  for (int i = 0; i < layers[layerNumber + 1].num_features; i++)
  {
    // layers[layerNumber].outputFeatures[node][i] = 0;
    for (int j = 0; j < layers[layerNumber].num_features; j++)
    {

      layers[layerNumber].outputFeatures[node][i] += (layers[layerNumber].inputFeatures[node][j] * layers[layerNumber].weights[j][i]);
      // std::cout<< layers[layerNumber].inputFeatures[node][i] <<"    "<< layers[layerNumber].weights[node][i]<<std::endl;
    }
    layers[layerNumber].outputFeatures[node][i] = relu(layers[layerNumber].outputFeatures[node][i] + layers[layerNumber + 1].bias[i]);
  }
}

// void backPropagation(GNN &gnn, int layerr)
// {
//   graph &g = gnn.getGraph();
//   std::vector<layer> &layers = gnn.getLayers();

//   // backpropagation for graph neural networks
//   #pragma omp parallel for schedule(dynamic)
//   for (int nod = 0; nod < g.num_nodes(); nod++)
//   {
//     for (auto edge : g.getNeighbors(nod))
//     {
//       for (int i = 0; i <layers[0].num_features; i++)
//       {
//         layers[0].grad_weights[edge.destination][i] += layers[0].grad_output[nod][i] * layers[0].outputFeatures[edge.destination][i];
//         layers[0].grad_bias[edge.destination] += layers[0].grad_output[nod][i];
//         layers[0].grad_input[nod][i] += layers[0].grad_output[nod][i] * layers[0].weights[edge.destination][i];
//       }
//     }
//   }
// }
// void getT_omp(GNN &gnn, int layer)
// {
//   std::vector<layer> &layers = gnn.getLayers();
//   for (int i = 0; i < layers[layer].num_features; i++)
//   {
//     for (int j = 0; j < layers[layer + 1].num_features; j++)
//     {
//       layers[layer].weights[i][j] = layers[layer].weights[i][j];
//     }
//   }
// }

// void adam(GNN &gnn, int layer, float lr, float beta1, float beta2, float epsilon){
//   std::vector<std::vector<float>> weights = gnn.getWeights(layer);
//   std::vector<std::vector<float>> biases = gnn.getBiases(layer);
//   std::vector<std::vector<float>> grad_weights = gnn.getGradWeights(layer);
//   std::vector<std::vector<float>> grad_biases = gnn.getGradBiases(layer);
//   std::vector<std::vector<float>> m_weights = gnn.getMWeights(layer);
//   std::vector<std::vector<float>> m_biases = gnn.getMBiases(layer);
//   std::vector<std::vector<float>> v_weights = gnn.getVWeights(layer);
//   std::vector<std::vector<float>> v_biases = gnn.getVBiases(layer);
//   int t = gnn.getT(layer);
//   // adam optimizer
//   #pragma omp parallel for schedule(dynamic)
//   for (int i = 0; i < weights.size(); i++)
//   {
//     for (int j = 0; j < weights[i].size(); j++)
//     {
//       m_weights[i][j] = beta1 * m_weights[i][j] + (1 - beta1) * grad_weights[i][j];
//       v_weights[i][j] = beta2 * v_weights[i][j] + (1 - beta2) * grad_weights[i][j] * grad_weights[i][j];
//       float m_hat = m_weights[i][j] / (1 - pow(beta1, t));
//       float v_hat = v_weights[i][j] / (1 - pow(beta2, t));
//       weights[i][j] -= lr * m_hat / (sqrt(v_hat) + epsilon);
//     }
//   }
//   #pragma omp parallel for schedule(dynamic)
//   for (int i = 0; i < biases.size(); i++)
//   {
//     m_biases[i] = beta1 * m_biases[i] + (1 - beta1) * grad_biases[i];
//     v_biases[i] = beta2 * v_biases[i] + (1 - beta2) * grad_biases[i] * grad_biases[i];
//     float m_hat = m_biases[i] / (1 - pow(beta1, t));
//     float v_hat = v_biases[i] / (1 - pow(beta2, t));
//     biases[i] -= lr * m_hat / (sqrt(v_hat) + epsilon);
//   }
//   //gnn.setT(layer, t + 1);
// }
