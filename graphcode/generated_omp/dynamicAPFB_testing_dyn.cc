#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<atomic>
#include<omp.h>
#include"../graph.hpp"

auto mod(int a , int b)
{
  return a - b * (a / b);

}
auto APFB(graph& g , int nc , int* rmatch , int* cmatch
)
{
  bool* modified=new bool[g.num_nodes()];
  bool* modified_nxt=new bool[g.num_nodes()];
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
            for (edge col_vertex_edge : g.getNeighbors(col_vertex)) 
            {int neigh_row = col_vertex_edge.destination ;
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
auto dynamicAPFB_testing_del(graph& g ,int nc, int previouslyMatched , bool* modified , std::vector<update> deleteBatch , 
  int* rmatch , int* cmatch)
{
  bool* modified_nxt=new bool[g.num_edges()];
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
  #pragma omp parallel for
  for(int i = 0 ; i < deleteBatch.size() ; i++)
  {
    update update = deleteBatch[i];
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
        for (int col_vertex = 0; col_vertex < g.num_nodes(); col_vertex ++) 
        {
          if (col_vertex < nc )
            {
            if (bfsArray[col_vertex] == bfsLevel )
              {
              for (edge col_vertex_edge : g.getNeighbors(col_vertex)) 
              {int neigh_row = col_vertex_edge.destination ;
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
        for (int row_vertex = 0; row_vertex < g.num_nodes(); row_vertex ++) 
        {
          if (row_vertex >= nc )
            {
            compress[row_vertex] = compress_next[row_vertex];
            compress_next[row_vertex] = false;
          }
        }
      }
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
  totalMatching = previouslyMatched - totalMatching;
  return totalMatching;

}
auto dynamicAPFB_testing_add(graph& g ,int nc, int previouslyMatched , bool* modified , std::vector<update> addBatch , 
  int* rmatch , int* cmatch)
{
  bool* modified_nxt=new bool[g.num_edges()];
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
  #pragma omp parallel for
  for(int i = 0 ; i < addBatch.size() ; i++)
  {
    update update = addBatch[i];
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
        for (int col_vertex = 0; col_vertex < g.num_nodes(); col_vertex ++) 
        {
          if (col_vertex < nc )
            {
            if (bfsArray[col_vertex] == bfsLevel )
              {
              for (edge col_vertex_edge : g.getNeighbors(col_vertex)) 
              {int neigh_row = col_vertex_edge.destination ;
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
        for (int row_vertex = 0; row_vertex < g.num_nodes(); row_vertex ++) 
        {
          if (row_vertex >= nc )
            {
            compress[row_vertex] = compress_next[row_vertex];
            compress_next[row_vertex] = false;
          }
        }
      }
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
  totalMatching = previouslyMatched + totalMatching;
  return totalMatching;

}
void DynAPFB(graph& g , int nc , std::vector<update> updateBatch , int batchSize
)
{
  int* rmatch=new int[g.num_nodes()];
  int* cmatch=new int[g.num_nodes()];
  int matchCount = APFB(g,nc,rmatch,cmatch);
  int batchElements = 0;
  for( int updateIndex = 0 ; updateIndex < updateBatch.size() ; updateIndex += batchSize){
    if((updateIndex + batchSize) > updateBatch.size())
    {
      batchElements = updateBatch.size() - updateIndex ;
    }
    else
    batchElements = batchSize ;
    bool* modified_del=new bool[g.num_edges()];
    #pragma omp parallel for
    for (int t = 0; t < g.num_edges(); t ++) 
    {
      modified_del[t] = false;
    }
    std::vector<update> deleteBatch = g.getDeletesFromBatch(updateIndex, batchSize, updateBatch);
    std::vector<update> addBatch = g.getAddsFromBatch(updateIndex, batchSize, updateBatch);
    for(int batchIndex = updateIndex ; batchIndex < (updateIndex+batchSize) && batchIndex < updateBatch.size() ; batchIndex++){
      if(updateBatch[batchIndex].type == 'd')
      {
        update u = updateBatch[batchIndex] ;
        int src = u.source;
        int dest = u.destination;
        for (edge src_edge : g.getNeighbors(src)) 
        {int nbr = src_edge.destination ;
          edge e = src_edge;
          if (nbr == dest )
            modified_del[e.id] = true;
        }

      }
    }
    matchCount = dynamicAPFB_testing_add(g,nc,matchCount,modified_del,addBatch,rmatch,cmatch);
    g.updateCSRDel(updateBatch, updateIndex, batchElements);

    g.updateCSRAdd(updateBatch, updateIndex, batchElements);

    bool* modified_add=new bool[g.num_edges()];
    #pragma omp parallel for
    for (int t = 0; t < g.num_edges(); t ++) 
    {
      modified_add[t] = false;
    }
    for(int batchIndex = updateIndex ; batchIndex < (updateIndex+batchSize) && batchIndex < updateBatch.size() ; batchIndex++){
      if(updateBatch[batchIndex].type == 'a')
      {
        update u = updateBatch[batchIndex] ;
        int src = u.source;
        int dest = u.destination;
        for (edge src_edge : g.getNeighbors(src)) 
        {int nbr = src_edge.destination ;
          edge e = src_edge;
          if (nbr == dest )
            modified_add[e.id] = true;
        }

      }
    }
    matchCount = dynamicAPFB_testing_del(g,nc,matchCount,modified_add,deleteBatch,rmatch,cmatch);
    free(modified_del) ;
    deleteBatch.clear();
    addBatch.clear();
    free(modified_add) ;

  }

}
