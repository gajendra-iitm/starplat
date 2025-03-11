#include"mst_dsl1.h"

void Boruvka(graph& g)
{
  int* nodeId=new int[g.num_nodes()];
  int* color=new int[g.num_nodes()];
  bool* isMSTEdge=new bool[g.num_edges()];
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    nodeId[t] = -1;
  }
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    color[t] = -1;
  }
  #pragma omp parallel for
  for (int t = 0; t < g.num_edges(); t ++) 
  {
    isMSTEdge[t] = false;
  }
  #pragma omp parallel for
  for (int u = 0; u < g.num_nodes(); u ++) 
  {
    nodeId[u] = u;
    color[u] = u;
  }
  bool* modified=new bool[g.num_nodes()];
  bool* modified_nxt=new bool[g.num_nodes()];
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    modified[t] = false;
    modified_nxt[t] = false;
  }
  int* minEdgeOfComp=new int[g.num_nodes()];
  int* minEdge=new int[g.num_nodes()];
  bool noNewComp = false;
  while ( !noNewComp )
  {
    noNewComp = true;
    #pragma omp parallel for
    for (int t = 0; t < g.num_nodes(); t ++) 
    {
      minEdge[t] = -1;
    }
    #pragma omp parallel for
    for (int src = 0; src < g.num_nodes(); src ++) 
    {
      for (int edge = g.indexofNodes[src]; edge < g.indexofNodes[src+1]; edge ++) 
      {int dst = g.edgeList[edge] ;
        if (color[src] != color[dst] )
          {
          int e = g.getEdge(dummy,dummy).id;
          int minEdge = minEdge[src];
          if (minEdge == -1 )
            {
            minEdge[src] = e;
          }
          if (minEdge != -1 )
            {
            int minDst = data[minEdge];
            if (weight[e] < weight[minEdge] || (weight[e] == weight[minEdge] && color[dst] < color[minDst]) )
              {
              minEdge[src] = e;
            }
          }
        }
      }
    }
    #pragma omp parallel for
    for (int t = 0; t < g.num_nodes(); t ++) 
    {
      minEdgeOfComp[t] = -1;
    }
    bool finishedMinEdge = false;
    while ( !finishedMinEdge )
    {
      finishedMinEdge = true;
      #pragma omp parallel for
      for (int u = 0; u < g.num_nodes(); u ++) 
      {
        int comp = color[u];
        int minEdge = minEdgeOfComp[comp];
        int e = minEdge[u];
        if (e != -1 )
          {
          int dst = data[e];
          if (minEdge == -1 )
            {
            minEdgeOfComp[comp] = e;
            finishedMinEdge = false;
          }
          if (minEdge != -1 )
            {
            int minDst = data[minEdge];
            if (weight[e] < weight[minEdge] || (weight[e] == weight[minEdge] && color[dst] < color[minDst]) )
              {
              minEdgeOfComp[comp] = e;
              finishedMinEdge = false;
            }
          }
        }
      }
    }
    #pragma omp parallel for
    for (int src = 0; src < g.num_nodes(); src ++) 
    {
      if (color[src] == nodeId[src] )
        {
        int srcMinEdge = minEdgeOfComp[src];
        if (srcMinEdge != -1 )
          {
          int dst = data[srcMinEdge];
          int dstLead = color[dst];
          int dstMinEdge = minEdgeOfComp[dstLead];
          if (dstMinEdge != -1 )
            {
            int dstOfDst = data[dstMinEdge];
            int dstOfDstLead = color[dstOfDst];
            if (color[src] == color[dstOfDstLead] && color[src] > color[dstLead] )
              {
              minEdgeOfComp[dstLead] = -1;
            }
          }
        }
      }
    }
    #pragma omp parallel for
    for (int src = 0; src < g.num_nodes(); src ++) 
    {
      if (color[src] == nodeId[src] )
        {
        int srcMinEdge = minEdgeOfComp[src];
        if (srcMinEdge != -1 )
          {
          isMSTEdge[srcMinEdge] = true;
        }
      }
    }
    #pragma omp parallel for
    for (int src = 0; src < g.num_nodes(); src ++) 
    {
      if (color[src] == nodeId[src] )
        {
        int srcMinEdge = minEdgeOfComp[src];
        if (srcMinEdge != -1 )
          {
          noNewComp = false;
          int dst = data[srcMinEdge];
          color[src] = color[dst];
        }
      }
    }
    bool finished = false;
    while ( !finished )
    {
      finished = true;
      #pragma omp parallel for
      for (int u = 0; u < g.num_nodes(); u ++) 
      {
        int my_color = color[u];
        int other_color = color[my_color];
        if (my_color != other_color )
          {
          finished = false;
          color[u] = other_color;
        }
      }
    }
  }

}
