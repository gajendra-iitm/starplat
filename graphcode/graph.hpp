#pragma once
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <string.h>
#include <climits>
#include <cmath>
#include <random>
#include "graph_ompv2.hpp"

class env;
extern env environment;

// using namespace std;

class edge
{
public:
  int32_t source;
  int32_t destination;
  double weight;
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
  float *edgeLen;
  float *diff_edgeLen;
  float *rev_edgeLen;
  float *diff_rev_edgeLen;
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

  graph(char *file);

  std::map<int, std::vector<edge>> getEdges();

  float *getEdgeLen();

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
  double getWeight(int src, int dest);
  void changeWeight(int src, int dest, double weight);
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
public:
  int32_t num_features;
  double **weights;
  double *bias;
  double **aggregatedFeatures;
  double **preActivatedFeatures;
  double **postActivatedFeatures;
  double **grad_pre_act_output;
  double **grad_weights;
  double *grad_bias;
  double **m_weights;
  double *m_biases;
  double **v_weights;
  double *v_biases;
};

class GNN
{
  graph &g;
  std::vector<layer> layers;
  std::vector<int32_t> labels;
  std::vector<std::vector<double>> features;
  char *feat_file, *lab_file;
  int num_features, num_classes,init;
  // std::vector<std::string> activationFunctions;

public:
  GNN(graph &g, char *feat_file, char *lab_file);
  graph &getGraph();
  void loadFeatures();
  void loadLabels();
  void gcnPreprocessing();
  int numFeatures();
  int initType();
  std::vector<std::vector<double>> &getFeatures();
  int numClasses();
  void initializeLayers(std::vector<int> neuronsPerLayer, char *initType);
  std::vector<layer> &getLayers();
  std::vector<int32_t> &getLabels();
  void aggregate(int node, int layerNumber);
  void forwardPass(int node, int layerNumber);
  void backPropogation(int layerNumber);
  void adamOptimizer(int epochNumber, double lr, double beta1, double beta2, double epsilon, double decay = 0.0);
  void predict();
};

class env
{
  char *backend;
  char *algoType;
  char *filename;

public:
  env(char *backend, char *algoType, char *filename);
  char *get_backend();
};
