#include"APFB_Matching.h"

void APFB(graph& g , int nc)
{
  bool* modified=new bool[g.num_nodes()];
  bool* modified_nxt=new bool[g.num_nodes()];
  #pragma omp parallel for
  for (int t = 0; t < g.num_nodes(); t ++) 
  {
    modified[t] = false;
    modified_nxt[t] = false;
  }
  int* rmatch=new int[g.num_nodes()];
  int* cmatch=new int[g.num_nodes()];
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
      std::cout << "Matching Results:\n";
    
    long match_count = 0LL;
    #pragma omp parallel for
    for (int r = 0; r < g.num_nodes(); r++) {
        if (r >= nc && rmatch[r] != -1) {
            ++match_count;
        }
    }
    // #pragma omp parallel for
    // for (int c = 0; c < nc; c++) {
    //     ++ match_count;
    // }
    std::cout << "Number of matches: " << match_count << std::endl;
}
