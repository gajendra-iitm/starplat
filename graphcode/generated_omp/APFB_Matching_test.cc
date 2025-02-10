#include"APFB_Matching_test.h"

auto mod(int a , int b)
{
  return a - b * (a / b);

}
auto APFB(graph& g , int nc)
{
  bool* modified=new bool[g.num_nodes()];
  bool* modified_nxt=new bool[g.num_nodes()];
  int* rmatch=new int[g.num_nodes()];
  int* cmatch=new int[g.num_nodes()];
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    modified[t] = false;
    modified_nxt[t] = false;
  }
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    rmatch[t] = -1;
  }
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    cmatch[t] = -1;
  }
  bool noNewPaths = false;
  int* bfsArray=new int[g.num_nodes()];
  int* predeccesor=new int[g.num_nodes()];
  bool* compress=new bool[g.num_nodes()];
  bool* compress_next=new bool[g.num_nodes()];
  bool* visited=new bool[g.num_nodes()];
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    visited[t] = false;
  }
  int evenComponents = 0;
  int oddComponents = 0;
  int unmatchedComponents = 0;
  while ( !noNewPaths )
  {
    noNewPaths = true;
    int L0 = 0;
    int NOT_VISITED = L0 - 1;
    #pragma omp parallel for
    for (int t = 0; t < g.num_nodes(); t ++) 
    {
      bfsArray[t] = NOT_VISITED;
    }
    #pragma omp parallel for
    for (int c = 0; c < g.num_nodes(); c ++) 
    {
      if (c < nc )
        {
        if (cmatch[c] == -1 )
          {
          bfsArray[c] = L0;
        }
      }
    }
    #pragma omp parallel for
    for (int t = 0; t < g.num_nodes(); t ++) 
    {
      predeccesor[t] = -1;
    }
    int bfsLevel = L0;
    bool noNewVertices = false;
    while ( !noNewVertices )
    {
      noNewVertices = true;
      #pragma omp parallel for
      for (int col_vertex = 0; col_vertex < g.num_nodes(); col_vertex ++) 
      {
        if (col_vertex < nc )
          {
          if (bfsArray[col_vertex] == bfsLevel )
            {
            for (int edge = g.indexofNodes[col_vertex]; edge < g.indexofNodes[col_vertex+1]; edge ++) 
            {int neigh_row = g.edgeList[edge] ;
              if (neigh_row >= nc )
                {
                int col_match = rmatch[neigh_row];
                if (col_match > -1 )
                  {
                  if (bfsArray[col_match] == NOT_VISITED )
                    {
                    noNewVertices = false;
                    bfsArray[col_match] = bfsLevel + 1;
                    predeccesor[neigh_row] = col_vertex;
                  }
                }
                if (col_match == -1 )
                  {
                  rmatch[neigh_row] = -2;
                  predeccesor[neigh_row] = col_vertex;
                  noNewPaths = false;
                }
              }
            }
          }
        }
      }
      bfsLevel = bfsLevel + 1;
    }
    #pragma omp parallel for
    for (int t = 0; t < g.num_nodes(); t ++) 
    {
      compress[t] = false;
    }
    #pragma omp parallel for
    for (int t = 0; t < g.num_nodes(); t ++) 
    {
      compress_next[t] = false;
    }
    #pragma omp parallel for
    for (int r = 0; r < g.num_nodes(); r ++) 
    {
      if (r >= nc && rmatch[r] == -2 )
        {
        compress[r] = true;
      }
    }
    bool compressed = false;
    while ( !compressed )
    {
      compressed = true;
      #pragma omp parallel for
      for (int row_vertex = 0; row_vertex < g.num_nodes(); row_vertex ++) 
      {
        if (compress[row_vertex] == true )
          {
          if (row_vertex >= nc )
            {
            int matched_col = predeccesor[row_vertex];
            int matched_row = cmatch[matched_col];
            bool isValid = true;
            if (matched_row != -1 )
              {
              if (predeccesor[matched_row] == matched_col )
                {
                isValid = false;
              }
            }
            if (isValid )
              {
              cmatch[matched_col] = row_vertex;
              rmatch[row_vertex] = matched_col;
              if (matched_row != -1 )
                {
                compress_next[matched_row] = true;
                compressed = false;
              }
            }
          }
        }
      }
      #pragma omp parallel for
      for (int row_vertex = 0; row_vertex < g.num_nodes(); row_vertex ++) 
      {
        if (row_vertex >= nc )
          {
          compress[row_vertex] = compress_next[row_vertex];
          compress_next[row_vertex] = false;
        }
      }
    }
    #pragma omp parallel for
    for (int r = 0; r < g.num_nodes(); r ++) 
    {
      if (r >= nc )
        {
        int c = rmatch[r];
        if (c >= 0 )
          {
          if (cmatch[c] != r )
            {
            rmatch[r] = -1;
          }
        }
        if (c == -2 )
          {
          rmatch[r] = -1;
        }
      }
    }
  }
  bool* in_bfs=new bool[g.num_nodes()];
  bool* isMatching=new bool[g.num_nodes()];
  int* bfsDist=new int[g.num_nodes()];
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    bfsDist[t] = 0;
  }
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    in_bfs[t] = false;
  }
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    isMatching[t] = false;
  }
  int componentSize = 0;
  int unmatchedCount = 0;
  int altEdgeCount = 0;
  #pragma omp parallel for
  for (int v = 0; v < g.num_nodes(); v ++) 
  {
    visited[v] = true;
    in_bfs[v] = true;
    isMatching[v] = (cmatch[v] != -1 || rmatch[v] != -1);
    bfsDist[v] = 0;
    std::vector<std::vector<int>> levelNodes(g.num_nodes()) ;
    std::vector<std::vector<int>>  levelNodes_later(omp_get_max_threads()) ;
    std::vector<int>  levelCount(g.num_nodes()) ;
    int phase = 0 ;
    levelNodes[phase].push_back(v) ;
    int bfsCount = 1 ;
    levelCount[phase] = bfsCount;
    while ( bfsCount > 0 )
    {
       int prev_count = bfsCount ;
      bfsCount = 0 ;
      #pragma omp parallel for
      for( int l = 0; l < prev_count ; l++)
      {
        int u = levelNodes[phase][l] ;
        for(int edge = g.indexofNodes[u] ; edge < g.indexofNodes[u+1] ; edge++) {
          int nbr = g.edgeList[edge] ;
          int dnbr ;
          if(bfsDist[nbr]<0)
          {
            dnbr = __sync_val_compare_and_swap(&bfsDist[nbr],-1,bfsDist[u]+1);
            if (dnbr < 0)
            {
              int num_thread = omp_get_thread_num();
               levelNodes_later[num_thread].push_back(nbr) ;
            }
          }
        }
        bool isMatchedEdge = (cmatch[u] != -1 || rmatch[u] != -1);
        for (int edge = g.indexofNodes[u]; edge < g.indexofNodes[u+1]; edge ++) 
        {int neigh = g.edgeList[edge] ;
          if(bfsDist[neigh]==bfsDist[u]+1)
          {
            if (!visited[neigh] )
              {
              bool nextIsMatchedEdge = !isMatchedEdge;
              if ((isMatchedEdge && (cmatch[u] == neigh || rmatch[u] == neigh)) || (!isMatchedEdge && (cmatch[u] == -1 || rmatch[u] == -1)) )
                {
                visited[neigh] = true;
                altEdgeCount = altEdgeCount + 1;
                isMatching[neigh] = nextIsMatchedEdge;
                in_bfs[neigh] = true;
              }
            }
          }
        }
      }
      phase = phase + 1 ;
      for(int i = 0;i < omp_get_max_threads();i++)
      {
         levelNodes[phase].insert(levelNodes[phase].end(),levelNodes_later[i].begin(),levelNodes_later[i].end());
         bfsCount = bfsCount+levelNodes_later[i].size();
         levelNodes_later[i].clear();
      }
       levelCount[phase] = bfsCount ;
    }
    phase = phase - 1 ;
    if (unmatchedCount == 0 && mod(altEdgeCount,2) == 0 )
      {
      evenComponents = evenComponents + 1;
    }
    else
    if (mod(altEdgeCount,2) == 1 )
      {
      oddComponents = oddComponents + 1;
    }
    else
    {
      unmatchedComponents = unmatchedComponents + 1;
    }
    int* bfsDist=new int[g.num_nodes()];
  }
  int totalMatching = 0;
  #pragma omp parallel for
  for (int v = 0; v < g.num_nodes(); v ++) 
  {
    if (rmatch[v] != -1 )
      {
      totalMatching = totalMatching + 1;
    }
  }
  return totalMatching;

}
