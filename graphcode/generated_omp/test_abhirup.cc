#include"test_abhirup.h"

auto compute_static_TC(graph& g)
{
  long staic_triangle_count = 0;
  #pragma omp parallel for reduction(+ : staic_triangle_count)
  for (int v = 0; v < g.num_nodes(); v ++) 
  {
    for (int edge = g.indexofNodes[v]; edge < g.indexofNodes[v+1]; edge ++) 
    {int u = g.edgeList[edge] ;
      if (v < u )
        {
        for (int edge = g.indexofNodes[v]; edge < g.indexofNodes[v+1]; edge ++) 
        {int w = g.edgeList[edge] ;
          if (v < w )
            {
            if (g.check_if_nbr(u, w) && u < w )
              {
              staic_triangle_count = staic_triangle_count + 1;
            }
          }
        }
      }
    }
  }
  return staic_triangle_count;

}
