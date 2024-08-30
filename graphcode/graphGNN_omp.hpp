#include "graph.hpp"

void xaviersInit_omp(float **weights, int num_neurons_current, int num_features_next)
{
  // xaiver's initialization
  double xavier = sqrt(6.0 / (num_neurons_current + num_features_next));
  std::random_device rd;  // Obtain a random number from hardware
  std::mt19937 gen(rd()); // Seed the generator
  std::uniform_real_distribution<> dist(-xavier, xavier);
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

void softmax(float *x, int size, float *y)
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
    sum += exp(x[i] - 1);
  }
  for (int i = 0; i < size; i++)
  {
    y[i] = exp(x[i]) / sum;
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
  layers[0].preActivatedFeatures = new float *[gnn.getGraph().num_nodes()];
  layers[0].postActivatedFeatures = new float *[gnn.getGraph().num_nodes()];
  for (int i = 0; i < gnn.getGraph().num_nodes(); i++)
  {
    layers[0].preActivatedFeatures[i] = new float[neuronsPerLayer[0]];
    layers[0].postActivatedFeatures[i] = new float[neuronsPerLayer[0]];
  }

  for (int i = 1; i < neuronsPerLayer.size(); i++)
  {
    layers[i].num_features = neuronsPerLayer[i];
    layers[i].weights = new float *[neuronsPerLayer[i - 1]];
    layers[i].m_weights = new float *[neuronsPerLayer[i - 1]];
    layers[i].m_biases = new float[neuronsPerLayer[i]];
    layers[i].v_weights = new float *[neuronsPerLayer[i - 1]];
    layers[i].v_biases = new float[neuronsPerLayer[i]];
    for (int j = 0; j < neuronsPerLayer[i - 1]; j++)
    {
      layers[i].weights[j] = new float[neuronsPerLayer[i]];
      layers[i].m_weights[j] = new float[neuronsPerLayer[i]];
      layers[i].v_weights[j] = new float[neuronsPerLayer[i]];
    }

    for (int j = 0; j < neuronsPerLayer[i - 1]; j++)
    {
      for (int k = 0; k < neuronsPerLayer[i]; k++)
      {
        layers[i].m_weights[j][k] = 0;
        layers[i].v_weights[j][k] = 0;
      }
    }

    for (int j = 0; j < neuronsPerLayer[i]; j++)
    {
      layers[i].m_biases[j] = 0;
      layers[i].v_biases[j] = 0;
    }

    layers[i].aggregatedFeatures = new float *[gnn.getGraph().num_nodes()];
    layers[i].preActivatedFeatures = new float *[gnn.getGraph().num_nodes()];
    layers[i].postActivatedFeatures = new float *[gnn.getGraph().num_nodes()];
    for (int j = 0; j < gnn.getGraph().num_nodes(); j++)
    {
      layers[i].aggregatedFeatures[j] = new float[neuronsPerLayer[i - 1]];
      layers[i].preActivatedFeatures[j] = new float[neuronsPerLayer[i]];
      layers[i].postActivatedFeatures[j] = new float[neuronsPerLayer[i]];
    }

    layers[i].bias = new float[neuronsPerLayer[i]];
    layers[i].grad_bias = new float[neuronsPerLayer[i]];
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
      layers[0].postActivatedFeatures[j][i] = features[j][i];
    }
  }

  if (strcmp(initType, "xaviers") == 0)
  {
    for (int i = 1; i < neuronsPerLayer.size(); i++)
    {
      xaviersInit_omp(layers[i].weights, neuronsPerLayer[i - 1], neuronsPerLayer[i]);
      // initialize bias wrt xaviers
      for (int j = 0; j < neuronsPerLayer[i]; j++)
      {
        layers[i].bias[j] = 0;
      }
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
    for (int i = 0; i < layers[layerNumber - 1].num_features; i++)
    {
      layers[layerNumber].aggregatedFeatures[node][i] += layers[layerNumber - 1].postActivatedFeatures[edge.destination][i] * edge.weight;
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
    layers[layerNumber].preActivatedFeatures[node][i] = 0;
    for (int j = 0; j < layers[layerNumber - 1].num_features; j++)
    {
      layers[layerNumber].preActivatedFeatures[node][i] += (layers[layerNumber].aggregatedFeatures[node][j] * layers[layerNumber].weights[j][i]);
    }
    layers[layerNumber].preActivatedFeatures[node][i] += layers[layerNumber].bias[i];
    if (layerNumber == layers.size() - 1)
    {
      softmax(layers[layerNumber].preActivatedFeatures[node], layers[layerNumber].num_features, layers[layerNumber].postActivatedFeatures[node]);
      return;
    }
    layers[layerNumber].postActivatedFeatures[node][i] = relu(layers[layerNumber].preActivatedFeatures[node][i]);
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

void backPropagation_omp(GNN &gnn, int layerNumber)
{
  graph &g = gnn.getGraph();
  std::vector<layer> &layers = gnn.getLayers();
  // std::cout << "Backpropagation" << std::endl;
  if (layerNumber == 0)
    return;

  std::vector<int32_t> y_true = gnn.getLabels();

  float **y_pred = layers[layers.size() - 1].preActivatedFeatures;

  if (layerNumber == layers.size() - 1)
  {
    for (int i = 0; i < y_true.size(); i++)
    {
      int c = y_true[i];
      std::cout << "y_pred : "<< c<< std::endl; 
      for (int j = 0; j < gnn.numClasses(); j++)
      {
        layers[layerNumber].grad_pre_act_output[i][j] = y_pred[i][j] - (c == j ? 1 : 0);
      }
    }
  }
  else
  {
    // grad_pre_act_output [layer != last] = (grad_pre_act_output [layer + 1] * W[layer + 1]^T) * relu_derivative(pre_act_output[layer])

    for (int nod = 0; nod < g.num_nodes(); nod++)
    {
      for (int i = 0; i < layers[layerNumber].num_features; i++)
      {
        layers[layerNumber].grad_pre_act_output[nod][i] = 0;
        for (int j = 0; j < layers[layerNumber + 1].num_features; j++)
        {
          layers[layerNumber].grad_pre_act_output[nod][i] += layers[layerNumber + 1].grad_pre_act_output[nod][j] * layers[layerNumber + 1].weights[i][j];
        }
      }
    }
    for (int i = 0; i < layers[layerNumber].num_features; i++)
    {
      for (int nod = 0; nod < g.num_nodes(); nod++)
      {
        layers[layerNumber].grad_pre_act_output[nod][i] *= derivative_relu(layers[layerNumber].preActivatedFeatures[nod][i]);
      }
    }
  }

  // grad_weights [layer] = (aggregatedFeatures[layer]^T * grad_pre_act_output[layer])
  for (int i = 0; i < layers[layerNumber - 1].num_features; i++)
  {
    for (int j = 0; j < layers[layerNumber].num_features; j++)
    {
      layers[layerNumber].grad_weights[i][j] = 0;
      for (int nod = 0; nod < g.num_nodes(); nod++)
      {
        layers[layerNumber].grad_weights[i][j] += layers[layerNumber].aggregatedFeatures[nod][i] * layers[layerNumber].grad_pre_act_output[nod][j];
        // if(layers[layerNumber].grad_weights[i][j] <-2)
        // {
        //   layers[layerNumber].grad_weights[i][j] = -2;
        // }
        // else if(layers[layerNumber].grad_weights[i][j] >2)
        // {
        //   layers[layerNumber].grad_weights[i][j] = 2;
        // }
      }
      layers[layerNumber].grad_weights[i][j] /= g.num_nodes();
    }
  }

  // grad_bias [layer] = sum(grad_pre_act_output[layer])
  for (int i = 0; i < layers[layerNumber].num_features; i++)
  {
    for (int nod = 0; nod < g.num_nodes(); nod++)
    {
      layers[layerNumber].grad_bias[i] += layers[layerNumber].grad_pre_act_output[nod][i];
    }
    layers[layerNumber].grad_bias[i] /= g.num_nodes();
  }

  // L1 norm
  // std::cout << "L1 norm for layer " << layerNumber << " : ";
  // for (int j = 0; j < layers[layerNumber - 1].num_features; j++)
  // {
  //   for (int i = 0; i < layers[layerNumber].num_features; i++)
  //   {
  //     layers[layerNumber].weights[j][i] -= (0.001 * layers[layerNumber].grad_weights[j][i]);
  //     // std::cout << layers[layerNumber].grad_weights[j][i] << " ";
  //   }
  //   // std::cout << std::endl;
  // }
  // // for bias
  // for (int i = 0; i < layers[layerNumber].num_features; i++)
  // {
  //   layers[layerNumber].bias[i] -= (0.001 * layers[layerNumber].grad_bias[i]);
  // }
}

void adamOptimizer_omp(GNN &gnn, int epochNumber, float lr, float beta1, float beta2, float epsilon)
{
  int t = epochNumber;
  std::vector<layer> &layers = gnn.getLayers();
  // std::cout << "OPTIMIZER STARTED" << std::endl;
  for (int i = 1; i < layers.size(); i++)
  {
    // std::cout << "OPTIMIZING LAYER " << i << std::endl;
    // adam optimizer
    // #pragma omp parallel for schedule(dynamic)
    for (int j = 0; j < layers[i - 1].num_features; j++)
    {
      for (int k = 0; k < layers[i].num_features; k++)
      {
        layers[i].m_weights[j][k] = beta1 * layers[i].m_weights[j][k] + (1 - beta1) * layers[i].grad_weights[j][k];
        layers[i].v_weights[j][k] = beta2 * layers[i].v_weights[j][k] + (1 - beta2) * layers[i].grad_weights[j][k] * layers[i].grad_weights[j][k];
        float m_hat = layers[i].m_weights[j][k] / (1 - pow(beta1, t));
        float v_hat = layers[i].v_weights[j][k] / (1 - pow(beta2, t));
        layers[i].weights[j][k] += lr * m_hat / (sqrt(v_hat) + epsilon);
      }
    }
    // std::cout << "WEIGHTS FOR LAYER " << i << " DONE" << std::endl;
    // #pragma omp parallel for schedule(dynamic)
    for (int j = 0; j < layers[j].num_features; j++)
    {
      layers[i].m_biases[j] = beta1 * layers[i].m_biases[j] + (1 - beta1) * layers[i].grad_bias[j];
      layers[i].v_biases[j] = beta2 * layers[i].v_biases[j] + (1 - beta2) * layers[i].grad_bias[j] * layers[i].grad_bias[j];
      float m_hat = layers[i].m_biases[j] / (1 - pow(beta1, t));
      float v_hat = layers[i].v_biases[j] / (1 - pow(beta2, t));
      layers[i].bias[j] += lr * m_hat / (sqrt(v_hat) + epsilon);
    }
    // std::cout << "BIASES FOR LAYER " << i << " DONE" << std::endl;
  }
}

void predict_omp(GNN &gnn)
{
  // store the index of largest value from the postactivationfeatures to predict the accuracy
  std::vector<layer> &layers = gnn.getLayers();
  graph &g = gnn.getGraph();
  std::vector<int32_t> y_true = gnn.getLabels();
  int correct = 0;
  for (int i = 0; i < g.num_nodes(); i++)
  {
    int max_index = 0;
    for (int j = 0; j < layers[layers.size() - 1].num_features; j++)
    {
      if (layers[layers.size() - 1].postActivatedFeatures[i][j] > layers[layers.size() - 1].postActivatedFeatures[i][max_index])
      {
        max_index = j;
      }
    }
    if (max_index == y_true[i])
    {
      correct++;
    }
  }
  std::cout << "Accuracy: " << (float)correct / g.num_nodes() << std::endl;
}
