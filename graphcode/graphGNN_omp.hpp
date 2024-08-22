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
    layers[i].aggregatedFeatures = new float *[gnn.getGraph().num_nodes()];
    layers[i].outputFeatures = new float *[gnn.getGraph().num_nodes()];
    for (int j = 0; j < gnn.getGraph().num_nodes(); j++)
    {
      layers[i].inputFeatures[j] = new float[neuronsPerLayer[i]];
      layers[i].aggregatedFeatures[j] = new float[neuronsPerLayer[i]];
      if (i < neuronsPerLayer.size() - 1)
        layers[i].outputFeatures[j] = new float[neuronsPerLayer[i + 1]];
    }

    layers[i].bias = new float[neuronsPerLayer[i + 1]];
    layers[i].grad_input = new float *[gnn.getGraph().num_nodes()];
    layers[i].grad_weights = new float *[neuronsPerLayer[i]];
    for (int j = 0; j < neuronsPerLayer[i]; j++)
    {
      layers[i].grad_weights[j] = new float[neuronsPerLayer[i + 1]];
      if (i > 0)
        layers[i].grad_input[j] = new float[neuronsPerLayer[i - 1]];
    }
    layers[i].grad_bias = new float[neuronsPerLayer[i + 1]];
    // layers[i].grad_output = new float[neuronsPerLayer[i + 1]];

    // std::cout << "layer with size " << layers[i].num_features << " initialized" << std::endl;
  }
  layers[neuronsPerLayer.size() - 1].num_features = neuronsPerLayer[neuronsPerLayer.size() - 1];
  layers[neuronsPerLayer.size() - 1].bias = new float[neuronsPerLayer[neuronsPerLayer.size() - 1]];
  layers[neuronsPerLayer.size() - 1].grad_bias = new float[neuronsPerLayer[neuronsPerLayer.size() - 1]];
  // layers[neuronsPerLayer.size() - 1].grad_output = new float[neuronsPerLayer[neuronsPerLayer.size() - 1]];
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

void calculategradinput_omp(GNN &gnn, int layerNumber)
{
  std::vector<layer> layers = gnn.getLayers();
  for (int nod = 0; nod < gnn.getGraph().num_nodes(); nod++)
  {
    for (auto edge : gnn.getGraph().getNeighbors(nod))
    {
      for (int i = 0; i < layers[layerNumber].num_features; i++)
      {
        layers[layerNumber].grad_input[nod][i] += layers[layerNumber].grad_output[edge.destination][i] * layers[layerNumber + 1].weights[i][edge.destination];
      }
    }
  }
}

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
      layers[layerNumber].aggregatedFeatures[node][i] += layers[layerNumber].inputFeatures[edge.destination][i] * edge.weight;
      std::cout << layers[0].aggregatedFeatures[node][i] << " ";
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
    layers[layerNumber].outputFeatures[node][i] = 0;
    for (int j = 0; j < layers[layerNumber].num_features; j++)
    {
      layers[layerNumber].outputFeatures[node][i] += (layers[layerNumber].aggregatedFeatures[node][j] * layers[layerNumber].weights[j][i]);
      // std::cout<< layers[layerNumber].inputFeatures[node][i] <<"    "<< layers[layerNumber].weights[node][i]<<std::endl;
    }
    if (layerNumber == layers.size() - 1)
    {
      layers[layerNumber].outputFeatures[node][i] = softmax(layers[layerNumber].outputFeatures[node], layers[layerNumber + 1].num_features);
    }
    else
      layers[layerNumber].outputFeatures[node][i] = relu(layers[layerNumber].outputFeatures[node][i] + layers[layerNumber + 1].bias[i]);
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

  // Loss_n (L_n) = 1/m (\sum{y_pred_n - y_n})
  // grad_weights_n = A_n-1^T * Loss_n
  // grad_bias_n = Loss_n
  // Loss_n-1 = Loss_n * W_n^T
  // grad_weights_n-1 = A_n-2^T * Loss_n-1
  // grad_bias_n-1 = Loss_n-1
  // Loss_n-2 = Loss_n-1 * W_n-1^T
  // grad_weights_n-2 = A_n-3^T * Loss_n-2
  // grad_bias_n-2 = Loss_n-2
  //...
  // Loss_1 = Loss_2 * W_2^T
  // grad_weights_1 = X^T * Loss_1
  // grad_bias_1 = Loss_1

  // backpropagation for graph neural networks
  // Loss = y_pred - y_true
  // Y_pred = last layer output
  // Y_true = labels
  std::vector<int32_t> y_true = gnn.getLabels();
  std::vector<std::vector<float>> y_pred = layers[layerNumber].outputFeatures;
  std::vector<std::vector<float>> Loss;
  if (layerNumber == layers.size() - 1)
  {
    Loss.resize(y_true.size());
    for (int i = 0; i < y_true.size(); i++)
    {
      Loss[i].resize(gnn.numClasses());
      int c = y_true[i];
      for (int j = 0; j < gnn.numClasses(); j++)
      {
        Loss[i][j] = y_pred[i][j] - (c == j ? 1 : 0);
      }
    }

    // grad_weights_n = A_n-1^T * Loss_n
    // grad_bias_n = Loss_n
    // Loss_n-1 = Loss_n * W_n^T

    for (int i = 0; i < layers[layerNumber].num_features; i++)
    {
      for (int j = 0; j < layers[layerNumber + 1].num_features; j++)
      {
        layers[layerNumber].grad_weights[i][j] = 0;
      }
    }

    for (int i = 0; i < layers[layerNumber + 1].num_features; i++)
    {
      layers[layerNumber].grad_bias[i] = 0;
    }

    for (int i = 0; i < layers[layerNumber].num_features; i++)
    {
      layers[layerNumber].grad_input[i] = 0;
    }

    for (int i = 0; i < layers[layerNumber].num_features; i++)
    {
      for (int j = 0; j < layers[layerNumber - 1].num_features; j++)
      {
        for (int nod = 0; nod < g.num_nodes(); nod++)
        {
          layers[layerNumber].grad_weights[i][j] += layers[layerNumber].outputFeatures[nod][i] * Loss[nod][j];
          layers[layerNumber].grad_input[nod][j] += Loss[nod][i] * layers[layerNumber].weights[j][i];
        }
      }
    }
  }
  else
  {
    for (int i = 0; i < layers[layerNumber].num_features; i++)
    {
      for (int j = 0; j < layers[layerNumber + 1].num_features; j++)
      {
        layers[layerNumber].grad_weights[i][j] = 0;
      }
    }

    for (int i = 0; i < layers[layerNumber + 1].num_features; i++)
    {
      layers[layerNumber].grad_bias[i] = 0;
    }

    for (int i = 0; i < layers[layerNumber].num_features; i++)
    {
      layers[layerNumber].grad_input[i] = 0;
    }

    for (int i = 0; i < layers[layerNumber].num_features; i++)
    {
      for (int j = 0; j < layers[layerNumber + 1].num_features; j++)
      {
        for (int nod = 0; nod < g.num_nodes(); nod++)
        {
          layers[layerNumber].grad_weights[i][j] += layers[layerNumber].outputFeatures[nod][i] * layers[layerNumber + 1].grad_input[nod][j];
          if (layerNumber > 0)
          {
            layers[layerNumber].grad_input[nod][j] += layers[layerNumber + 1].grad_input[nod][i] * layers[layerNumber + 1].weights[i][j];
            if (j == 0)
              layers[layerNumber].grad_bias[i] += Loss[nod][i];
          }
        }
      }
    }
  }
}

// void backPropagation(GNN &gnn, int layerNumber)
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

// void getT_omp(GNN &gnn, int layerNumber)
// {
//   std::vector<layer> &layers = gnn.getLayers();
//   for (int i = 0; i < layers[layerNumber].num_features; i++)
//   {
//     for (int j = 0; j < layers[layerNumber + 1].num_features; j++)
//     {
//       layers[layerNumber].weights[i][j] = layers[layerNumber].weights[i][j];
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
