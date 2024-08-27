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

void softmax(float *x, int size)
{
  // float max = x[0];
  // for (int i = 1; i < size; i++)
  // {
  //   if (x[i] > max)
  //   {
  //     max = x[i];
  //   }
  // }
  // float sum = 0;
  // for (int i = 0; i < size; i++)
  // {
  //   x[i] = exp(x[i] - max);
  //   sum += x[i];
  // }
  // for (int i = 0; i < size; i++)
  // {
  //   x[i] = x[i] / sum;
  // }
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

void initializeLayers_omp(GNN &gnn, std::vector<int32_t> neuronsPerHiddenLayer, char *initType)
{
  std::vector<int32_t> neuronsPerLayer;
  std::cout << "Neurons initially " << gnn.numFeatures() << std::endl;
  neuronsPerLayer.push_back(gnn.numFeatures());
  for (int i = 0; i < neuronsPerHiddenLayer.size(); i++)
  {
    neuronsPerLayer.push_back(neuronsPerHiddenLayer[i]);
  }
  neuronsPerLayer.push_back(gnn.numClasses());
  std::vector<layer> &layers = gnn.getLayers();
  layers.resize(neuronsPerLayer.size());
  layers[0].num_features = neuronsPerLayer[0];
  layers[0].outputFeatures = new float *[gnn.getGraph().num_nodes()];
  for (int i = 0; i < gnn.getGraph().num_nodes(); i++)
  {
    layers[0].outputFeatures[i] = new float[neuronsPerLayer[0]];
  }

  for (int i = 1; i < neuronsPerLayer.size(); i++)
  {
    layers[i].num_features = neuronsPerLayer[i];
    layers[i].weights = new float *[neuronsPerLayer[i - 1]];

    for (int j = 0; j < neuronsPerLayer[i - 1]; j++)
    {
      layers[i].weights[j] = new float[neuronsPerLayer[i]];
    }

    layers[i].aggregatedFeatures = new float *[gnn.getGraph().num_nodes()];
    layers[i].outputFeatures = new float *[gnn.getGraph().num_nodes()];
    for (int j = 0; j < gnn.getGraph().num_nodes(); j++)
    {
      layers[i].aggregatedFeatures[j] = new float[neuronsPerLayer[i - 1]];
      layers[i].outputFeatures[j] = new float[neuronsPerLayer[i]];
    }

    layers[i].bias = new float[neuronsPerLayer[i]];
    layers[i].grad_weights = new float *[neuronsPerLayer[i - 1]];
    for (int j = 0; j < neuronsPerLayer[i - 1]; j++)
    {
      layers[i].grad_weights[j] = new float[neuronsPerLayer[i]];
    }

    layers[i].grad_pre_act_output = new float *[gnn.getGraph().num_nodes()];
    layers[i].grad_post_act_output = new float *[gnn.getGraph().num_nodes()];
    for (int j = 0; j < gnn.getGraph().num_nodes(); j++)
    {
      layers[i].grad_pre_act_output[j] = new float[neuronsPerLayer[i]];
      layers[i].grad_post_act_output[j] = new float[neuronsPerLayer[i]];
    }

    std::cout << "layer with size " << layers[i].num_features << " initialized" << std::endl;
  }
  // layers[neuronsPerLayer.size() - 1].grad_output = new float[neuronsPerLayer[neuronsPerLayer.size() - 1]];
  std::vector<std::vector<float>> features = gnn.getFeatures();

  for (int j = 0; j < gnn.getGraph().num_nodes(); j++)
  {
    for (int i = 0; i < gnn.numFeatures(); i++)
    {
      layers[0].outputFeatures[j][i] = features[j][i];
    }
  }

  if (strcmp(initType, "xaviers") == 0)
  {
    for (int i = 1; i < neuronsPerLayer.size(); i++)
    {
      xaviersInit_omp(layers[i].weights, neuronsPerLayer[i - 1], neuronsPerLayer[i]);
    }
  }
  else if (strcmp(initType, "normal") == 0)
  {
    for (int i = 1; i < neuronsPerLayer.size(); i++)
    {
      normalDistributionInit_omp(layers[i].weights, neuronsPerLayer[i - 1], neuronsPerLayer[i]);
    }
  }
  else if (strcmp(initType, "uniform") == 0)
  {
    for (int i = 1; i < neuronsPerLayer.size(); i++)
    {
      uniformDistributionInit_omp(layers[i].weights, neuronsPerLayer[i - 1], neuronsPerLayer[i]);
    }
  }
  else
  {
    std::cout << "Invalid initialization type" << std::endl;
  }
  std::cout << "Layers initialized" << std::endl;
}

void getWeights_omp(GNN &gnn, int layerNumber)
{
  std::vector<layer> &layers = gnn.getLayers();
  for (int i = 0; i < layers[layerNumber].num_features; i++)
  {
    for (int j = 0; j < layers[layerNumber + 1].num_features; j++)
    {
      layers[layerNumber].weights[i][j] = layers[layerNumber].weights[i][j];
    }
  }
}

// void calculategradinput_omp(GNN &gnn, int layerNumber)
// {
//   std::vector<layer> layers = gnn.getLayers();
//   for (int nod = 0; nod < gnn.getGraph().num_nodes(); nod++)
//   {
//     for (auto edge : gnn.getGraph().getNeighbors(nod))
//     {
//       for (int i = 0; i < layers[layerNumber].num_features; i++)
//       {
//         layers[layerNumber].grad_input[nod][i] += layers[layerNumber].grad_output[edge.destination][i] * layers[layerNumber + 1].weights[i][edge.destination];
//       }
//     }
//   }
// }

void aggregate_omp(GNN &gnn, int node, int layerNumber)
{
  graph &g = gnn.getGraph();
  std::vector<layer> &layers = gnn.getLayers();

  for (int i = 0; i < layers[layerNumber].num_features; i++)
  {
    layers[layerNumber].aggregatedFeatures[node][i] = 0;
  }

  for (auto edge : g.getNeighbors(node))
  {
    // int c = 0;
    for (int i = 0; i < layers[layerNumber].num_features; i++)
    {
      layers[layerNumber].aggregatedFeatures[node][i] += layers[layerNumber - 1].outputFeatures[edge.destination][i] * edge.weight;
    }
  }
}

void forwardPass_omp(GNN &gnn, int node, int layerNumber) //, char *aggregationType)
{
  std::vector<layer> &layers = gnn.getLayers();
  graph &g = gnn.getGraph();

  if (layerNumber == 0)
  {
    return;
  }

  aggregate_omp(gnn, node, layerNumber);

  for (int i = 0; i < layers[layerNumber].num_features; i++)
  {
    layers[layerNumber].outputFeatures[node][i] = 0;
    for (int j = 0; j < layers[layerNumber - 1].num_features; j++)
    {
      layers[layerNumber].outputFeatures[node][i] += (layers[layerNumber].aggregatedFeatures[node][j] * layers[layerNumber].weights[j][i]);
    }
    layers[layerNumber].outputFeatures[node][i] = (layers[layerNumber].outputFeatures[node][i] + layers[layerNumber].bias[i]);
    if (layerNumber == layers.size() - 1)
    {
      softmax(layers[layerNumber].outputFeatures[node], layers[layerNumber].num_features);
      return;
    }
    layers[layerNumber].outputFeatures[node][i] = relu(layers[layerNumber].outputFeatures[node][i]);
  }
}

/*
Forward Pass
    MatrixXd Z1 = X * W1 + b1;
    MatrixXd A1 = relu(Z1);

    MatrixXd Z2 = A1 * W2 + b2;
    MatrixXd A2 = relu(Z2);

    MatrixXd Z3 = A2 * W3 + b3;
    MatrixXd y_pred = softmax(Z3);

// Compute Loss
    double loss = cross_entropy_loss(y_pred, y_true);
    cout << "Loss: " << loss << endl;

    // Backward Pass
    MatrixXd dZ3 = y_pred - y_true;  // Gradient of loss w.r.t. logits

    MatrixXd dW3 = A2.transpose() * dZ3;
    MatrixXd db3 = dZ3.colwise().sum();

    MatrixXd dA2 = dZ3 * W3.transpose();
    MatrixXd dZ2 = dA2.array() * relu_derivative(Z2).array();

    MatrixXd dW2 = A1.transpose() * dZ2;
    MatrixXd db2 = dZ2.colwise().sum();

    MatrixXd dA1 = dZ2 * W2.transpose();
    MatrixXd dZ1 = dA1.array() * relu_derivative(Z1).array();

    MatrixXd dW1 = X.transpose() * dZ1;
    MatrixXd db1 = dZ1.colwise().sum();

    // Gradient Descent Update
    W3 -= learning_rate * dW3;
    b3 -= learning_rate * db3;
    W2 -= learning_rate * dW2;
    b2 -= learning_rate * db2;
    W1 -= learning_rate * dW1;
    b1 -= learning_rate * db1;
 */

// void backPropagation_omp(GNN &gnn, int layerNumber)
// {
//   graph &g = gnn.getGraph();
//   std::vector<layer> &layers = gnn.getLayers();
//   std::cout << "Backpropagation" << std::endl;
//   if (layerNumber == 0)
//     return;

//   std::vector<int32_t> y_true = gnn.getLabels();

//   float **y_pred = layers[layers.size() - 1].outputFeatures;
//   std::cout << "grad_loss_pre_act_output" << std::endl;
//   if (layerNumber == layers.size() - 1)
//   {
//     for (int i = 0; i < y_true.size(); i++)
//     {
//       int c = y_true[i];
//       for (int j = 0; j < gnn.numClasses(); j++)
//       {
//         layers[layerNumber].grad_pre_act_output[i][j] = y_pred[i][j] - (c == j ? 1 : 0);
//       }
//     }
//   }
//   else
//   {
//     // grad_pre_act_output [layer != last] = (Loss_n * W_n^T) * relu_derivative(pre_act_output_n)
//   }
//   std::cout << "Init_values_grad" << std::endl;

//   for (int i = 0; i < layers[layerNumber].num_features; i++)
//   {
//     for (int j = 0; j < layers[layerNumber + 1].num_features; j++)
//     {
//       layers[layerNumber].grad_weights[i][j] = 0;
//     }
//   }

//   for (int i = 0; i < layers[layerNumber + 1].num_features; i++)
//   {
//     layers[layerNumber].grad_post_act_output = 0;
//   }

//   for (int i = 0; i < layers[layerNumber - 1].num_features; i++)
//   {
//     for (int j = 0; j < layers[layerNumber].num_features; j++)
//     {
//       for (int nod = 0; nod < g.num_nodes(); nod++)
//       {
//         layers[layerNumber - 1].grad_weights[i][j] += layers[layerNumber].outputFeatures[nod][i] * layers[layerNumber].grad_pre_act_output[nod][j];
//         layers[layerNumber + 1].grad_input[nod][j] += layers[layerNumber].grad_pre_act_output[nod][i] * layers[layerNumber + 1].weights[j][i];
//       }
//     }
//   }
// }

// void adam(GNN &gnn, int layerNumber, float lr, float beta1, float beta2, float epsilon){
//   std::vector<std::vector<float>> weights = gnn.getWeights(layerNumber);
//   std::vector<std::vector<float>> biases = gnn.getBiases(layerNumber);
//   std::vector<std::vector<float>> grad_weights = gnn.getGradWeights(layerNumber);
//   std::vector<std::vector<float>> grad_biases = gnn.getGradBiases(layerNumber);
//   std::vector<std::vector<float>> m_weights = gnn.getMWeights(layerNumber);
//   std::vector<std::vector<float>> m_biases = gnn.getMBiases(layerNumber);
//   std::vector<std::vector<float>> v_weights = gnn.getVWeights(layerNumber);
//   std::vector<std::vector<float>> v_biases = gnn.getVBiases(layerNumber);
//   int t = gnn.getT(layerNumber);
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
