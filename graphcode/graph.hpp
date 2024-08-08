#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <string.h>
#include <climits>
#include "graph_ompv2.hpp"

#ifndef GRAPH_H
#define GRAPH_H
#include "graphGNN_omp.hpp"

class env;
extern env environment;

// using namespace std;

class edge
{
public:
  int32_t source;
  int32_t destination;
  int32_t weight;
  int32_t id; /* -unique Id for each edge.
                 -useful in adding properties to edges. */
  int dir;
};

// bool counter=true;
class graph
{
private:
  int32_t nodesTotal;
  int32_t edgesTotal;
  int32_t *edgeLen;
  int32_t *diff_edgeLen;
  int32_t *rev_edgeLen;
  int32_t *diff_rev_edgeLen;
  char *filePath;
  std::map<int32_t, std::vector<edge>> edges;

public:
  int32_t *indexofNodes;     /* stores prefix sum for outneighbours of a node*/
  int32_t *rev_indexofNodes; /* stores prefix sum for inneighbours of a node*/
  int32_t *edgeList;         /*stores destination corresponding to edgeNo.
                               required for iteration over out neighbours */
  int32_t *srcList;          /*stores source corresponding to edgeNo.
                               required for iteration over in neighbours */
  std::vector<edge> graph_edge;
  int32_t *diff_indexofNodes;
  int32_t *diff_edgeList;
  int32_t *diff_rev_indexofNodes;
  int32_t *diff_rev_edgeList;
  int32_t *perNodeCSRSpace;
  int32_t *perNodeRevCSRSpace;
  int32_t *edgeMap;
  std::map<int, int> outDeg;
  std::map<int, int> inDeg;

  graph(char *file)
  {
    filePath = file;
    nodesTotal = 0;
    edgesTotal = 0;
    diff_edgeList = NULL;
    diff_indexofNodes = NULL;
    diff_rev_indexofNodes = NULL;
    diff_rev_edgeList = NULL;
    rev_edgeLen = NULL;
  }

  std::map<int, std::vector<edge>> getEdges();

  int *getEdgeLen();

  int num_nodes();

  // function to get total number of edges
  int num_edges();

  std::vector<edge> getInOutNbrs(int v);

  edge getEdge(int s, int d);

  // library function to check candidate vertex is in the path from root to dest in SPT.
  bool inRouteFromSource(int candidate, int dest, int *parent);

  bool check_if_nbr(int s, int d);
  int common_nbrscount(int node1, int node2);
  int getOutDegree(int v);

  int getInDegree(int v);
  void addEdge(int src, int dest, int aks);
  void delEdge(int src, int dest);
  std::vector<update> parseUpdates(char *updateFile);
  std::vector<update> getDeletesFromBatch(int updateIndex, int batchSize, std::vector<update> updateVec);
  std::vector<update> getAddsFromBatch(int updateIndex, int batchSize, std::vector<update> updateVec);
  void propagateNodeFlags(bool *modified);
  void parseEdges();
  void parseEdgesResidual();
  void parseGraphResidual();
  void parseGraph();
  /******************************|| Dynamic Graph Libraries ||********************************/

  void updateCSRDel(std::vector<update> &batchUpdate, int k, int size);
  void updateCSRAdd(std::vector<update> &batchUpdate, int k, int size);
  std::vector<edge> getNeighbors(int node);
  std::vector<edge> getInNeighbors(int node);
};

class layer
{
  int32_t num_features;
  float *weights;
  float bias;
  float *output;
  float *input;
  float *grad_input;
  float *grad_weights;
  float *grad_bias;
  float *grad_output;
};

class GNN
{
  graph &g;
  std::vector<layer> layers;
  std::vector<float> features;
  std::vector<int32_t> labels;
  char *featFile, *labFile;

public:
  GNN(graph &g, char *featFile, char *labFile) : g(g), featFile(featFile), labFile(labFile);
  graph &getGraph();
  void loadFeatures();
  void loadLabels();

  // void initializeLayers(std::vector<int> layers, std::string init_type)
  // {
  //   for (int i = 0; i < layers.size(); i++)
  //   {
  //     layer l;
  //     l.num_features = layers[i];
  //     l.weights = new float[l.num_features];
  //     l.bias = 0;
  //     l.output = new float[l.num_features];
  //     l.input = new float[l.num_features];
  //     l.grad_input = new float[l.num_features];
  //     l.grad_weights = new float[l.num_features];
  //     l.grad_bias = 0;
  //     l.grad_output = new float[l.num_features];

  //     if (init_type == "Xavier_transform")
  //     {
  //       for (int j = 0; j < l.num_features; j++)
  //       {
  //         l.weights[j] = 1.0 / sqrt(l.num_features);
  //       }
  //     }

  //     layers.push_back(l);
  //   }
  // }

  // void gcn_preprocessing()
  // {
  //   if (environment.get_backend() == "omp")
  //   {
  //     gcn_preprocessing_omp(this);
  //   }
  //   else if (environment.get_backend() == "cuda")
  //   {
  //     // preprocessing_cuda();
  //   }
  // }

  // void forward()
  // {
  //   for (int i = 0; i < layers.size(); i++)
  //   {
  //     layer l = layers[i];
  //     for (int j = 0; j < l.num_features; j++)
  //     {
  //       l.output[j] = 0;
  //       for (int k = 0; k < l.num_features; k++)
  //       {
  //         l.output[j] += l.weights[k] * l.input[k];
  //       }
  //       l.output[j] += l.bias;
  //     }
  //   }
  // }

  // void message_pass()
  // {
  //   env *instance = env::get_instance();
  //   if (instance->get_backend() == "omp")
  //   {
  //     message_pass_omp(this);
  //   }
  //   else if (instance->get_backend() == "cuda")
  //   {
  //     message_pass_cuda();
  //   }
  // }

  // void backward()
  // {
  //   for (int i = layers.size() - 1; i >= 0; i--)
  //   {
  //     layer l = layers[i];
  //     for (int j = 0; j < l.num_features; j++)
  //     {
  //       l.grad_input[j] = 0;
  //       for (int k = 0; k < l.num_features; k++)
  //       {
  //         l.grad_input[j] += l.grad_output[k] * l.weights[k];
  //         l.grad_weights[j] += l.grad_output[k] * l.input[j];
  //       }
  //       l.grad_bias += l.grad_output[j];
  //     }
  //   }
  // }
};

class env
{
  char *backend;
  char *algoType;
  char *filename;

public:
  env(char *backend, char *algoType, char *filename) char *get_backend();
};
#endif