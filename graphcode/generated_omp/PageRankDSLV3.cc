#include"PageRankDSLV3.h"

void ComputePageRank(graph& g , float beta , float delta , int maxIter , 
  float* pageRank)
{
  float numNodes = (float)g.num_nodes();
  float* pageRankNext=new float[g.num_nodes()];
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    pageRank[t] = 1 / numNodes;
    pageRankNext[t] = 0;
  }
  int iterCount = 0;
  float diff = 0.0 ;
  do
  {
    diff = 0.000000;
    #pragma omp parallel for reduction(+ : diff,diff)
    for (int v = 0; v < g.num_nodes(); v ++) 
    {
      float sum = 0.000000;
      for (int edge = g.rev_indexofNodes[v]; edge < g.rev_indexofNodes[v+1]; edge ++) 
      {int nbr = g.srcList[edge] ;
        sum = sum + pageRank[nbr] / (g.indexofNodes[nbr+1]-g.indexofNodes[nbr]);
      }
      float newPageRank = (1 - delta) / numNodes + delta * sum;
      if (newPageRank - pageRank[v] >= 0 )
        {
        diff = diff + newPageRank - pageRank[v];
      }
      else
      {
        diff = diff + pageRank[v] - newPageRank;
      }
      pageRankNext[v] = newPageRank;
    }
    #pragma omp parallel for
    for (int node = 0; node < g.num_nodes(); node ++) 
    {
      pageRank [node] = pageRankNext [node] ;
    }
    iterCount++;
  }
  while((diff > beta) && (iterCount < maxIter));
}
