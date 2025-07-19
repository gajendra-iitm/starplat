#ifndef STARPLAT_GRAPH_H
#define STARPLAT_GRAPH_H

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <queue>
#include <string>
#include <climits>
#include<cmath>
#include <random>
#include <unordered_set>
#include "graph_ompv2.hpp"
#include <stdlib.h>
#include "abstractGraph.hpp"

#ifdef __CUDACC__
#include "CUDA_GNN.cuh"
#else
#include "OMP_GNN.hpp"
#endif




//bool counter=true;
class graph : public AbstractGraph
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

  ~graph(){
    if (edgeLen) delete[] edgeLen;
    if (edgeList) delete[] edgeList;
    if (srcList) delete[] srcList;
  }

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

  graph copyGraph(){
    graph g_copy((char*)"");
    g_copy.nodesTotal = nodesTotal;
    g_copy.edgesTotal = edgesTotal;

    if (edgeLen) {
      g_copy.edgeLen = new int32_t[edgesTotal];
      std::copy(edgeLen, edgeLen + edgesTotal, g_copy.edgeLen);
    } else {
      g_copy.edgeLen = nullptr;
    }

    if (filePath) {
      g_copy.filePath = strdup(filePath);
    } else {
      g_copy.filePath = nullptr;
    }

    g_copy.edges = edges;

    if (indexofNodes) {
      g_copy.indexofNodes = new int32_t[nodesTotal + 2];
      std::copy(indexofNodes, indexofNodes + nodesTotal + 2, g_copy.indexofNodes);
    } else {
      g_copy.indexofNodes = nullptr;
    }


    if (edgeList) {
      g_copy.edgeList = new int32_t[edgesTotal];
      std::copy(edgeList, edgeList + edgesTotal, g_copy.edgeList);
    } else {
      g_copy.edgeList = nullptr;
    }

    if (srcList) {
      g_copy.srcList = new int32_t[edgesTotal];
      std::copy(srcList, srcList + edgesTotal, g_copy.srcList);
    } else {
      g_copy.srcList = nullptr;
    }

    g_copy.graph_edge = graph_edge;

    g_copy.outDeg = outDeg;
    g_copy.inDeg = inDeg;

    return g_copy;
  }

  std::map<int, std::vector<edge>> getEdges()
  {
    return edges;
  }

  int *getEdgeLen()
  {
    return edgeLen;
  }

  int num_nodes()
  {
    return nodesTotal + 1; // change it to nodesToTal
  }

  // function to get total number of edges
  int num_edges()
  {
    if (diff_edgeList != NULL)
    {
      return (edgesTotal + diff_indexofNodes[nodesTotal + 1]);
    }
    else
      return edgesTotal;
  }

  std::vector<edge> getInOutNbrs(int v)
  {

    std::vector<edge> resVec;

    std::vector<edge> inEdges = getInNeighbors(v);
    resVec.insert(resVec.end(), inEdges.begin(), inEdges.end());
    std::vector<edge> Edges = getNeighbors(v);
    resVec.insert(resVec.end(), Edges.begin(), Edges.end());

    return resVec;
  }

  edge getEdge(int s, int d)
  {

    int startEdge = indexofNodes[s];
    int endEdge = indexofNodes[s + 1] - 1;
    edge foundEdge;

    for (edge e : getNeighbors(s))
    {

      int nbr = e.destination;
      if (nbr == d)
      {
        return e;
      }
    }

    return foundEdge; // TODO: Maybe return a default value?
  }

  void randomShuffle(){
  
    auto rd = std::random_device {}; 
    auto rng = std::default_random_engine {rd()};
    for(int i=0;i<=nodesTotal;i++)
      {
        std::vector<edge>& edgeOfVertex=edges[i];
        std::shuffle(edgeOfVertex.begin(),edgeOfVertex.end(), rng);
      }

    std::shuffle(graph_edge.begin(), graph_edge.end(), rng);
    int edge_no=0;

    for(int i=0;i<=nodesTotal;i++) //change to 1-nodesTotal.
    {
      std::vector<edge> edgeofVertex=edges[i];
      indexofNodes[i]=edge_no;
      std::vector<edge>::iterator itr;
      for(itr=edgeofVertex.begin();itr!=edgeofVertex.end();itr++)
      {
        edgeList[edge_no]=(*itr).destination;
        edgeLen[edge_no]=(*itr).weight;
        edge_no++;
      }
    }
    indexofNodes[nodesTotal+1]=edge_no;//change to nodesTotal+1.
    
  }


  // library function to check candidate vertex is in the path from root to dest in SPT.
  bool inRouteFromSource(int candidate, int dest, int *parent)
  {

    while (parent[dest] != -1)
    {
      if (parent[dest] == candidate)
        return true;

      dest = parent[dest];
    }

    return false;
  }

  bool check_if_nbr(int s, int d)
  {
    /*int startEdge=indexofNodes[s];
    int endEdge=indexofNodes[s+1]-1;

    if(edgeList[startEdge]==d)
        return true;
    if(edgeList[endEdge]==d)
       return true;

    int mid;

    while(startEdge<=endEdge)
      {
       mid = (startEdge+endEdge)/2;

        if(edgeList[mid]==d)
           return true;

        if(d<edgeList[mid])
           endEdge=mid-1;
        else
          startEdge=mid+1;


      }*/

    /* int start = 0;
      int end = edges[s].size()-1;
      int mid;

       while(start<end)
      {
       mid = (start+end)/2;

        if(edges[s][mid].destination==d)
           return true;

        if(d<edges[s][mid].destination)
           end=mid-1;
        else
          start=mid+1;


      }*/

    for (edge e : getNeighbors(s))
    {
      int nbr = e.destination;
      if (nbr == d)
        return true;
    }

    return false;
  }

  int common_nbrscount(int node1, int node2)
  {
    int count = 0;
    int a = indexofNodes[node1 + 1];
    int b = indexofNodes[node2 + 1];
    int i = indexofNodes[node1];
    int j = indexofNodes[node2];

    while (i < a && j < b)
    {
      int n = edgeList[i];
      int m = edgeList[j];

      if (n == m)
      {
        i++;
        j++;
        count++;
      }
      else if (n < m)
        i++;
      else
        j++;
    }

    return count;
  }

  int getOutDegree(int v)
  {

    return outDeg[v];
  }

  int getInDegree(int v)
  {

    return inDeg[v];
  }

  void addEdge(int src, int dest, int aks)
  {
    int startIndex = indexofNodes[src];
    int endIndex = indexofNodes[src + 1];
    int nbrsCount = endIndex - startIndex;
    int insertAt = 0;

    if (edgeList[startIndex] >= dest || nbrsCount == 0)
      insertAt = startIndex;
    else if (edgeList[endIndex - 1] <= dest)
      insertAt = endIndex;
    else
    {

      for (int i = startIndex; i < endIndex - 1; i++) // find the correct index to insert.
      {
        if (edgeList[i] <= dest && edgeList[i + 1] >= dest)
        {
          insertAt = i + 1;
          break;
        }
      }
    }

    edgeList = (int32_t *)realloc(edgeList, sizeof(int32_t) * (edgesTotal + 1));
    edgeLen = (int32_t *)realloc(edgeLen, sizeof(int32_t) * (edgesTotal + 1));

    for (int i = edgesTotal - 1; i >= insertAt; i--) // shift the elements
    {
      edgeList[i + 1] = edgeList[i];
      edgeLen[i + 1] = edgeLen[i];
      // edgeMap[i+1] = edgeMap[i];
    }

    edgeList[insertAt] = dest;
    edgeLen[insertAt] = aks; // to be changed. the weight should be from paramters.

// update the CSR offset array.
#pragma omp parallel for
    for (int i = src + 1; i <= nodesTotal + 1; i++)
    {
      indexofNodes[i] += 1;
    }

    edge newEdge;
    newEdge.source = src;
    newEdge.destination = dest;
    newEdge.weight = aks;
    edges[src].push_back(newEdge);
    edgesTotal++;
  }

  void delEdge(int src, int dest)
  {
    int startEdge = indexofNodes[src];
    int endEdge = indexofNodes[src + 1] - 1;
    int mid;

    while (startEdge <= endEdge)
    {
      mid = (startEdge + endEdge) / 2;

      if (edgeList[mid] == dest)
        break;

      if (dest < edgeList[mid])
        endEdge = mid - 1;
      else
        startEdge = mid + 1;
    }

    /* int startEdge=rev_indexofNodes[dest];
      int endEdge=rev_indexofNodes[dest+1]-1;
      int mid ;

    while(startEdge<=endEdge)
     {
      mid = (startEdge+endEdge)/2;

       if(srcList[mid]==src)
          break;

       if(src<srcList[mid])
          endEdge=mid-1;
       else
         startEdge=mid+1;


     }
    */

    edgeLen[mid] = INT_MAX / 2;

    printf("src %d dest %d mid %d\n", src, dest, mid);
  }

  std::vector<update> parseUpdates(char *updateFile)
  {

    std::vector<update> update_vec = parseUpdateFile(updateFile);
    return update_vec;
  }

  std::vector<update> getDeletesFromBatch(int updateIndex, int batchSize, std::vector<update> updateVec)
  {
    std::vector<update> deleteVec = getDeletions(updateIndex, batchSize, updateVec);
    return deleteVec;
  }

  std::vector<update> getAddsFromBatch(int updateIndex, int batchSize, std::vector<update> updateVec)
  {
    std::vector<update> addVec = getAdditions(updateIndex, batchSize, updateVec);
    return addVec;
  }

  void propagateNodeFlags(bool *modified)
  {

    bool finished = false;
    while (!finished)
    {
      finished = true;

      for (int v = 0; v <= nodesTotal; v++)
        for (edge e : getNeighbors(v))
        {
          if (!modified[e.destination])
          {
            modified[e.destination] = true;
            finished = false;
          }
        }
    }
  }

  void parseEdges()
  {
    // printf("OH HELLOHIHod \n");
    std::ifstream infile;
    infile.open(filePath);
    std::string line;
    std::map<std::pair<int, int>, int> mpp;
    while (std::getline(infile, line))
    {
      if (line.length() == 0 || line[0] < '0' || line[0] > '9')
      {
        continue;
      }

      std::stringstream ss(line);
      edge e;
      int32_t source;
      int32_t destination;
      int32_t weightVal;

      ss >> source;
      if (source > nodesTotal)
        nodesTotal = source;

      ss >> destination;
      if (destination > nodesTotal)
        nodesTotal = destination;

      ss >> weightVal;

      std::pair<int, int> p;
      p.first = source;
      p.second = destination;
      mpp[p] += weightVal;
    }

    for (auto it = mpp.begin(); it != mpp.end(); ++it)
    {
      std::pair<int, int> key = it->first;
      int value = it->second;
      edge e;
      e.source = key.first;
      e.destination = key.second;
      e.weight = value;

      edgesTotal++;
      edges[e.source].push_back(e);
      graph_edge.push_back(e);
    }

    infile.close();
  }

  void parseEdgesResidual()
  {
    // printf("OH HELLOHIHod \n");
    std::ifstream infile;
    infile.open(filePath);
    std::string line;
    std::map<std::pair<int, int>, int> mpp;
    while (std::getline(infile, line))
    {
      if (line.length() == 0 || line[0] < '0' || line[0] > '9')
      {
        continue;
      }

      std::stringstream ss(line);
      edge e;
      int32_t source;
      int32_t destination;
      int32_t weightVal;

      ss >> source;
      if (source > nodesTotal)
        nodesTotal = source;

      ss >> destination;
      if (destination > nodesTotal)
        nodesTotal = destination;

      ss >> weightVal;

      std::pair<int, int> p;
      p.first = source;
      p.second = destination;
      mpp[p] += weightVal;
      p.first = destination;
      p.second = source;
      mpp[p] += 0;
    }

    for (auto it = mpp.begin(); it != mpp.end(); ++it)
    {
      std::pair<int, int> key = it->first;
      int value = it->second;
      edge e;
      e.source = key.first;
      e.destination = key.second;
      e.weight = value;

      edgesTotal++;
      edges[e.source].push_back(e);
      graph_edge.push_back(e);
    }

    infile.close();
  }

  void parseGraphResidual()
  {

    parseEdgesResidual();

// printf("Here half\n");
// printf("HELLO AFTER THIS %d \n",nodesTotal);
#pragma omp parallel for
    for (int i = 0; i <= nodesTotal; i++) // change to 1-nodesTotal.
    {
      std::vector<edge> &edgeOfVertex = edges[i];

      sort(edgeOfVertex.begin(), edgeOfVertex.end(),
           [](const edge &e1, const edge &e2)
           {
             if (e1.source != e2.source)
               return e1.source < e2.source;

             return e1.destination < e2.destination;
           });
    }

    indexofNodes = new int32_t[nodesTotal + 2];
    rev_indexofNodes = new int32_t[nodesTotal + 2];
    edgeList = new int32_t[edgesTotal]; // new int32_t[edgesTotal] ;
    srcList = new int32_t[edgesTotal];
    edgeLen = new int32_t[edgesTotal]; // new int32_t[edgesTotal] ;
    edgeMap = new int32_t[edgesTotal];
    perNodeCSRSpace = new int32_t[nodesTotal + 1];
    perNodeRevCSRSpace = new int32_t[nodesTotal + 1];
    int *edgeMapInter = new int32_t[edgesTotal];
    int *vertexInter = new int32_t[edgesTotal];

    int edge_no = 0;

    /* Prefix Sum computation for out neighbours
       Loads indexofNodes and edgeList.
    */
    for (int i = 0; i <= nodesTotal; i++) // change to 1-nodesTotal.
    {
      std::vector<edge> edgeofVertex = edges[i];

      indexofNodes[i] = edge_no;

      std::vector<edge>::iterator itr;

      for (itr = edgeofVertex.begin(); itr != edgeofVertex.end(); itr++)
      {

        edgeList[edge_no] = (*itr).destination;

        edgeLen[edge_no] = (*itr).weight;
        edge_no++;
      }

      perNodeCSRSpace[i] = 0;
      perNodeRevCSRSpace[i] = 0;
    }

    indexofNodes[nodesTotal + 1] = edge_no; // change to nodesTotal+1.

#pragma omp parallel for num_threads(4)
    for (int i = 0; i < nodesTotal + 1; i++)
      rev_indexofNodes[i] = 0;

    /* Prefix Sum computation for in neighbours
       Loads rev_indexofNodes and srcList.
    */

    /* count indegrees first */
    int32_t *edge_indexinrevCSR = new int32_t[edgesTotal];

#pragma omp parallel for num_threads(4)
    for (int i = 0; i <= nodesTotal; i++)
    {

      for (int j = indexofNodes[i]; j < indexofNodes[i + 1]; j++)
      {
        int dest = edgeList[j];
        int temp = __sync_fetch_and_add(&rev_indexofNodes[dest], 1);
        edge_indexinrevCSR[j] = temp;
      }
    }

    /* convert to revCSR */
    int prefix_sum = 0;
    for (int i = 0; i <= nodesTotal; i++)
    {
      int temp = prefix_sum;
      prefix_sum = prefix_sum + rev_indexofNodes[i];
      rev_indexofNodes[i] = temp;
    }
    rev_indexofNodes[nodesTotal + 1] = prefix_sum;

    /* store the sources in srcList */
#pragma omp parallel for num_threads(4)
    for (int i = 0; i <= nodesTotal; i++)
    {
      for (int j = indexofNodes[i]; j < indexofNodes[i + 1]; j++)
      {
        int dest = edgeList[j];
        int index_in_srcList = rev_indexofNodes[dest] + edge_indexinrevCSR[j];
        srcList[index_in_srcList] = i;
        edgeMapInter[index_in_srcList] = j;                        // RevCSR to CSR edge mapping.
        vertexInter[index_in_srcList] = srcList[index_in_srcList]; /*store the original content of srcList
                                                                    before sorting srcList.
                                                                    */
      }
    }

#pragma omp parallel for num_threads(4)
    for (int i = 0; i <= nodesTotal; i++)
    {
      std::vector<int> vect;
      vect.insert(vect.begin(), srcList + rev_indexofNodes[i], srcList + rev_indexofNodes[i + 1]);
      std::sort(vect.begin(), vect.end());
      for (int j = 0; j < vect.size(); j++)
        srcList[j + rev_indexofNodes[i]] = vect[j];
      int srcListIndex;

      for (int j = 0; j < vect.size(); j++)
      {
        srcListIndex = j + rev_indexofNodes[i];
        for (int k = 0; k < vect.size(); k++)
        {
          if (vertexInter[k + rev_indexofNodes[i]] == srcList[srcListIndex])
          {
            edgeMap[srcListIndex] = edgeMapInter[k + rev_indexofNodes[i]];
            break;
          }
        }
      }
      vect.clear();
    }

    for (int i = 0; i <= nodesTotal; i++)
    {

      inDeg[i] = rev_indexofNodes[i + 1] - rev_indexofNodes[i];
      outDeg[i] = indexofNodes[i + 1] - indexofNodes[i];
    }
    free(vertexInter);
    free(edgeMapInter);
    // change to nodesTotal+1.
    //  printf("hello after this %d %d\n",nodesTotal,edgesTotal);
  }

  void parseEdgesContent(){
    #pragma omp parallel for
    for (int i = 0; i <= nodesTotal; i++) // change to 1-nodesTotal.
    {
      std::vector<edge> &edgeOfVertex = edges[i];

      sort(edgeOfVertex.begin(), edgeOfVertex.end(),
           [](const edge &e1, const edge &e2)
           {
             if (e1.source != e2.source)
               return e1.source < e2.source;

             return e1.destination < e2.destination;
           });
    }

    indexofNodes = new int32_t[nodesTotal + 2];
    rev_indexofNodes = new int32_t[nodesTotal + 2];
    edgeList = new int32_t[edgesTotal]; // new int32_t[edgesTotal] ;
    srcList = new int32_t[edgesTotal];
    edgeLen = new int32_t[edgesTotal]; // new int32_t[edgesTotal] ;
    edgeMap = new int32_t[edgesTotal];
    perNodeCSRSpace = new int32_t[nodesTotal + 1];
    perNodeRevCSRSpace = new int32_t[nodesTotal + 1];
    int *edgeMapInter = new int32_t[edgesTotal];
    int *vertexInter = new int32_t[edgesTotal];

    int edge_no = 0;

    /* Prefix Sum computation for out neighbours
       Loads indexofNodes and edgeList.
    */
    for (int i = 0; i <= nodesTotal; i++) // change to 1-nodesTotal.
    {
      std::vector<edge> edgeofVertex = edges[i];

      indexofNodes[i] = edge_no;

      std::vector<edge>::iterator itr;

      for (itr = edgeofVertex.begin(); itr != edgeofVertex.end(); itr++)
      {

        edgeList[edge_no] = (*itr).destination;

        edgeLen[edge_no] = (*itr).weight;
        edge_no++;
      }

      perNodeCSRSpace[i] = 0;
      perNodeRevCSRSpace[i] = 0;
    }

    indexofNodes[nodesTotal + 1] = edge_no; // change to nodesTotal+1.

#pragma omp parallel for num_threads(4)
    for (int i = 0; i < nodesTotal + 1; i++)
      rev_indexofNodes[i] = 0;

    /* Prefix Sum computation for in neighbours
       Loads rev_indexofNodes and srcList.
    */

    /* count indegrees first */
    int32_t *edge_indexinrevCSR = new int32_t[edgesTotal];

#pragma omp parallel for num_threads(4)
    for (int i = 0; i <= nodesTotal; i++)
    {

      for (int j = indexofNodes[i]; j < indexofNodes[i + 1]; j++)
      {
        int dest = edgeList[j];
        int temp = __sync_fetch_and_add(&rev_indexofNodes[dest], 1);
        edge_indexinrevCSR[j] = temp;
      }
    }

    
      /* convert to revCSR */
      int prefix_sum = 0;
      for(int i=0;i<=nodesTotal;i++)
        {
          int temp = prefix_sum;
          prefix_sum = prefix_sum + rev_indexofNodes[i];
          rev_indexofNodes[i]=temp;
        }
        rev_indexofNodes[nodesTotal+1] = prefix_sum;

    /* store the sources in srcList */
#pragma omp parallel for num_threads(4)
    for (int i = 0; i <= nodesTotal; i++)
    {
      for (int j = indexofNodes[i]; j < indexofNodes[i + 1]; j++)
      {
        int dest = edgeList[j];
        int index_in_srcList = rev_indexofNodes[dest] + edge_indexinrevCSR[j];
        srcList[index_in_srcList] = i;
        edgeMapInter[index_in_srcList] = j;                        // RevCSR to CSR edge mapping.
        vertexInter[index_in_srcList] = srcList[index_in_srcList]; /*store the original content of srcList
                                                                    before sorting srcList.
                                                                    */
      }
    }

#pragma omp parallel for num_threads(4)
    for (int i = 0; i <= nodesTotal; i++)
    {
      std::vector<int> vect;
      vect.insert(vect.begin(), srcList + rev_indexofNodes[i], srcList + rev_indexofNodes[i + 1]);
      std::sort(vect.begin(), vect.end());
      for (int j = 0; j < vect.size(); j++)
        srcList[j + rev_indexofNodes[i]] = vect[j];
      int srcListIndex;

      for (int j = 0; j < vect.size(); j++)
      {
        srcListIndex = j + rev_indexofNodes[i];
        for (int k = 0; k < vect.size(); k++)
        {
          if (vertexInter[k + rev_indexofNodes[i]] == srcList[srcListIndex])
          {
            edgeMap[srcListIndex] = edgeMapInter[k + rev_indexofNodes[i]];
            break;
          }
        }
      }
      vect.clear();
    }

    for (int i = 0; i <= nodesTotal; i++)
    {

      inDeg[i] = rev_indexofNodes[i + 1] - rev_indexofNodes[i];
      outDeg[i] = indexofNodes[i + 1] - indexofNodes[i];
    }
    free(vertexInter);
    free(edgeMapInter);

  }

  void parseGraph()
  {
    parseEdges();
    parseEdgesContent();
  }

  /******************************|| Dynamic Graph Libraries ||********************************/

  void updateCSRDel(std::vector<update> &batchUpdate, int k, int size)
  {
    int num_nodes = nodesTotal + 1;
    std::vector<std::pair<int, int>> perNodeUpdateInfo;
    std::vector<std::pair<int, int>> perNodeUpdateRevInfo;
    std::vector<update> slicedUpdates;
    if (rev_edgeLen == NULL)
      rev_edgeLen = new int[edgesTotal];
    for (int i = 0; i < num_nodes; i++)
    {
      perNodeUpdateInfo.push_back({0, 0});
      perNodeUpdateRevInfo.push_back({0, 0});
    }

    // printf("size %d \n", size);
    /* perNode bookkeeping of updates and deletions */

    for (int i = 0; i < size; i++)
    {
      int pos = k + i;
      update u = batchUpdate[pos];
      int source = u.source;
      int destination = u.destination;
      char type = u.type;
      if (type == 'a')
      {
        perNodeUpdateInfo[source].second++;
        perNodeUpdateRevInfo[destination].second++;
      }
      else
      {
        perNodeUpdateInfo[source].first++;
        perNodeUpdateRevInfo[destination].first++;
      }

      slicedUpdates.push_back(u);
    }

    /* edge weights book-keeping for reverse CSR */

#pragma omp parallel for
    for (int i = 0; i < edgesTotal; i++)
    {
      /* int e = edgeMap[i];
       int weight = edgeLen[e];*/
      rev_edgeLen[i] = 1; // weight;
    }

    updateCSRDel_omp(num_nodes, edgesTotal, indexofNodes, edgeList, rev_indexofNodes, srcList, edgeLen, diff_edgeLen,
                     diff_indexofNodes, diff_edgeList, diff_rev_indexofNodes, diff_rev_edgeList, rev_edgeLen,
                     diff_rev_edgeLen, perNodeUpdateInfo, perNodeUpdateRevInfo, perNodeCSRSpace, perNodeRevCSRSpace, slicedUpdates);

    perNodeUpdateInfo.clear();
    perNodeUpdateRevInfo.clear();

    for (int i = 0; i <= nodesTotal; i++)
    {

      inDeg[i] = getInNeighbors(i).size();
      outDeg[i] = getNeighbors(i).size();
    }
  }

  void updateCSRAdd(std::vector<update> &batchUpdate, int k, int size)
  {
    int num_nodes = nodesTotal + 1;

    std::vector<std::pair<int, int>> perNodeUpdateInfo;
    std::vector<std::pair<int, int>> perNodeUpdateRevInfo;
    std::vector<update> slicedUpdates;

    for (int i = 0; i < num_nodes; i++)
    {
      perNodeUpdateInfo.push_back({0, 0});
      perNodeUpdateRevInfo.push_back({0, 0});
    }

    for (int i = 0; i < size; i++)
    {
      int pos = k + i;
      update u = batchUpdate[pos];
      int source = u.source;
      int destination = u.destination;
      char type = u.type;
      if (type == 'a')
      {
        perNodeUpdateInfo[source].second++;
        perNodeUpdateRevInfo[destination].second++;
      }
      else
      {
        perNodeUpdateInfo[source].first++;
        perNodeUpdateRevInfo[destination].first++;
      }

      slicedUpdates.push_back(u);
    }

    updateCSRAdd_omp(num_nodes, edgesTotal, indexofNodes, edgeList, rev_indexofNodes, srcList, edgeLen, &diff_edgeLen,
                     &diff_indexofNodes, &diff_edgeList, &diff_rev_indexofNodes, &diff_rev_edgeList, rev_edgeLen,
                     &diff_rev_edgeLen, perNodeUpdateInfo, perNodeUpdateRevInfo, perNodeCSRSpace, perNodeRevCSRSpace, slicedUpdates);

    perNodeUpdateInfo.clear();
    perNodeUpdateRevInfo.clear();

    for (int i = 0; i <= nodesTotal; i++)
    {

      inDeg[i] = getInNeighbors(i).size();
      outDeg[i] = getNeighbors(i).size();
    }
  }

  std::vector<edge> getNeighbors(int node)
  {

    std::vector<edge> out_edges;

    for (int i = indexofNodes[node]; i < indexofNodes[node + 1]; i++)
    {
      int nbr = edgeList[i];
      if (nbr != INT_MAX / 2)
      {
        edge e;
        e.source = node;
        e.destination = nbr;
        e.weight = this->edgeLen[i];
        e.id = i;
        e.dir = 1;
        //  printf(" weight %d\n", e.weight);
        out_edges.push_back(e);
      }
    }

    if (diff_edgeList != NULL)
    {
      for (int j = diff_indexofNodes[node]; j < diff_indexofNodes[node + 1]; j++)
      {
        int nbr = diff_edgeList[j];
        if (nbr != INT_MAX / 2)
        {
          edge e;
          e.source = node;
          e.destination = nbr;
          e.weight = diff_edgeLen[j];
          e.id = edgesTotal + j;
          e.dir = 1;
          // printf(" weight %d\n", e.weight);
          out_edges.push_back(e);
        }
      }
    }

    return out_edges;
  }

  std::vector<edge> getInNeighbors(int node)
  {

    std::vector<edge> in_edges;

    for (int i = rev_indexofNodes[node]; i < rev_indexofNodes[node + 1]; i++)
    {
      int nbr = srcList[i];
      if (nbr != INT_MAX / 2)
      {
        edge e;
        e.source = node;
        e.destination = nbr;
        e.weight = rev_edgeLen[i];
        in_edges.push_back(e);
        e.dir = 0;
      }
    }

    if (diff_rev_edgeList != NULL)
    {
      for (int j = diff_rev_indexofNodes[node]; j < diff_rev_indexofNodes[node + 1]; j++)
      {
        int nbr = diff_rev_edgeList[j];
        if (nbr != INT_MAX / 2)
        {
          edge e;
          e.source = node;
          e.destination = nbr;
          e.weight = diff_rev_edgeLen[j];
          in_edges.push_back(e);
          e.dir = 0;
        }
      }
    }

    return in_edges;
  }

  void parseAdjacencyList(std::map<int, std::vector<edge>>& graph){
    std::map<std::pair<int, int>, int> mpp;
    for (auto &kv : graph) {
      int node = kv.first;
      std::vector<edge> edges = kv.second;
      for (auto &e : edges) {
        mpp[{node, e.destination}] += e.weight;
      }
    }

    for (auto it = mpp.begin(); it != mpp.end(); ++it)
    {
      std::pair<int, int> key = it->first;
      int value = it->second;
      edge e;
      e.source = key.first;
      e.destination = key.second;
      e.weight = value;

      edgesTotal++;
      edges[e.source].push_back(e);
      graph_edge.push_back(e);
    }
    parseEdgesContent();  
  }

  graph getMST() {
    int V = nodesTotal + 1;
    std::vector<bool> inMST(V, false);
    std::vector<int> key(V, INT_MAX);
    std::vector<int> parent(V, -1);

    typedef std::pair<int, int> P;
    std::priority_queue<P, std::vector<P>, std::greater<P>> pq;

    key[0] = 0;
    pq.push({0, 0});

    graph mstGraph((char*)"");
    mstGraph.nodesTotal = 0;
    mstGraph.edgesTotal = 0;

    std::map<int, std::vector<edge>> tempMST;
    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();
        if (inMST[u])
            continue;
        inMST[u] = true;
        if (parent[u] != -1) {
            edge edgeTemp;
            edgeTemp.source = u;
            edgeTemp.destination = parent[u];
            edgeTemp.weight = key[u];
            tempMST[u].push_back(edgeTemp);

            edge edgeTemp2;
            edgeTemp2.destination = u;
            edgeTemp2.source = parent[u];
            edgeTemp2.weight = key[u];
            tempMST[parent[u]].push_back(edgeTemp2);
        }
        std::vector<edge> neighbors = this->getNeighbors(u);
        for (auto &e : neighbors) {
            int v = e.destination;
            int w = e.weight;
            if (!inMST[v] && w < key[v]) {
                key[v] = w;
                parent[v] = u;
                pq.push({key[v], v});
            }
        }
    }

    mstGraph.nodesTotal = tempMST.size();
    mstGraph.parseAdjacencyList(tempMST);

    return mstGraph;
}
  void setNodes(int nodes){
    this->nodesTotal=nodes;
  }

  void printGraph()
  {
    for (int i = 0; i < nodesTotal; i++)
    {
      std::cout << "Node " << i << ": ";
      for (edge e : getNeighbors(i))
      {
        std::cout << "(" << e.destination << ", " << e.weight << ") ";
      }
      std::cout << std::endl;
    }
  }


//Function to sample specified number of neighbors of a node in O(Sample Size) time.
std::vector<int> RandomSampleNeighbors(int node, int sample_size, int seed)
  {
      const int start = indexofNodes[node];
      const int nodeDeg = indexofNodes[node + 1] - start;
      
      // Handle edge cases and adjust sample size
      if (sample_size <= 0 || nodeDeg <= 0) return {};
      const int actual_sample_size = std::min(sample_size, nodeDeg);
      
      std::vector<int> sampled_neighbours;
      sampled_neighbours.reserve(actual_sample_size);
      
      // Case 1: Return all neighbors
      if(nodeDeg <= sample_size) {
          for (int i = 0; i < nodeDeg; ++i) {
              sampled_neighbours.push_back(edgeList[start + i]);
          }
          return sampled_neighbours;
      }
      
      std::mt19937 gen(seed);
      
      // Case 2: Small sample relative to degree - use direct sampling
      if (sample_size <= nodeDeg / 10) {
          std::unordered_set<int> selected_indices;
          selected_indices.reserve(actual_sample_size * 2); // Avoid rehashing
          
          while (sampled_neighbours.size() < actual_sample_size) {
              int idx = gen() % nodeDeg;
              if (selected_indices.insert(idx).second) { // If insertion was successful (new element)
                  sampled_neighbours.push_back(edgeList[start + idx]);
              }
          }
      }
      // Case 3: Large sample - use reservoir sampling
      else {
          // Fill with first k elements
          for (int i = 0; i < actual_sample_size; ++i) {
              sampled_neighbours.push_back(edgeList[start + i]);
          }
          
          // Apply reservoir sampling for remaining elements
          for (int i = actual_sample_size; i < nodeDeg; ++i) {
              int j = gen() % (i + 1);
              if (j < actual_sample_size) {
                  sampled_neighbours[j] = edgeList[start + i];
              }
          }
      }
      
      return sampled_neighbours;
  }
  
};

struct Point
{
  double x;
  double y;

  Point() {};
  Point(double x, double y) : x(x), y(y) {};
};

struct Edge
{
  int p1;
  int p2;

  Edge() {};
  Edge(int p1, int p2) : p1(p1), p2(p2) {};
};

struct Triangle
{
  int p1;
  int p2;
  int p3;

  Triangle() {};
  Triangle(int p1, int p2, int p3) : p1(p1), p2(p2), p3(p3) {};
};

class GNN
{
  public:
    GNN(){};

    void init(std:: vector<int> neuronsPerLayer, std::string initWeights,std::string folderPath){

      #ifdef __CUDACC__
        init_cuda(neuronsPerLayer, initWeights , folderPath);
      #else
        init_omp(neuronsPerLayer, initWeights , folderPath);
      #endif
    }

    void forward(std::string modelType, std::string aggregationType){
      #ifdef __CUDACC__
        forward_cuda(modelType, aggregationType);
      #else
        forward_omp(modelType, aggregationType);
      #endif
    }

    void backward(std::string modelType, std::string aggregationType, int epoch){
      #ifdef __CUDACC__
        backprop_cuda(modelType, aggregationType, epoch);
      #else
        backprop_omp(modelType, aggregationType, epoch);
      #endif
    }

    // void optimizer(std::string optimizerType, double learningRate){
    //   #ifdef __CUDACC__
    //     optimizer_cuda(optimizerType, learningRate);
    //   #else
    //     optimizer_omp(optimizerType, learningRate);
    //   #endif
    // }


    double compute_accuracy() 
    {
      #ifdef __CUDACC__
         compute_accuracy_cuda();
      #else
        return  compute_accuracy_omp();
      #endif
    }

    double compute_loss()
    {
      #ifdef __CUDACC__
         compute_loss_cuda();
      #else
        return compute_loss_omp();
      #endif
    }
};

#endif