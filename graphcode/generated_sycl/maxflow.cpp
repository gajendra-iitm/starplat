#ifndef GENCPP_MAXFLOW_H
#define GENCPP_MAXFLOW_H
#include <CL/sycl.hpp>
#include <iostream>
#include <fstream>
#include "../graph.hpp"
using namespace sycl;

void maxflow(graph& g,int s,int t)
{
  queue Q(default_selector_v);
  std::cout << "Selected device: " << Q.get_device().get_info<info::device::name>() << std::endl;
  // CSR BEGIN
  int V = g.num_nodes();
  int E = g.num_edges();

  printf("#nodes:%d\n",V);
  printf("#edges:%d\n",E);
  int* edgeLen = g.getEdgeLen();

  int *h_meta;
  int *h_data;
  int *h_src;
  int *h_weight;
  int *h_rev_meta;

  h_meta = (int *)malloc( (V+1)*sizeof(int));
  h_data = (int *)malloc( (E)*sizeof(int));
  h_src = (int *)malloc( (E)*sizeof(int));
  h_weight = (int *)malloc( (E)*sizeof(int));
  h_rev_meta = (int *)malloc( (V+1)*sizeof(int));

  for(int i=0; i<= V; i++) {
    int temp = g.indexofNodes[i];
    h_meta[i] = temp;
    temp = g.rev_indexofNodes[i];
    h_rev_meta[i] = temp;
  }

  for(int i=0; i< E; i++) {
    int temp = g.edgeList[i];
    h_data[i] = temp;
    temp = g.srcList[i];
    h_src[i] = temp;
    temp = edgeLen[i];
    h_weight[i] = temp;
  }


  int* d_meta;
  int* d_data;
  int* d_src;
  int* d_weight;
  int* d_rev_meta;
  bool* d_modified_next;

  d_meta=malloc_device<int>((1+V), Q);
  d_data=malloc_device<int>((E), Q);
  d_src=malloc_device<int>((E), Q);
  d_weight=malloc_device<int>((E), Q);
  d_rev_meta=malloc_device<int>((V+1), Q);
  d_modified_next=malloc_device<bool>((V), Q);

  Q.submit([&](handler &h)
    { h.memcpy(  d_meta,   h_meta, sizeof(int)*(V+1)); })
  .wait();
  Q.submit([&](handler &h)
    { h.memcpy(  d_data,   h_data, sizeof(int)*(E)); })
  .wait();
  Q.submit([&](handler &h)
    { h.memcpy(   d_src,    h_src, sizeof(int)*(E)); })
  .wait();
  Q.submit([&](handler &h)
    { h.memcpy(d_weight, h_weight, sizeof(int)*(E)); })
  .wait();
  Q.submit([&](handler &h)
    { h.memcpy(d_rev_meta, h_rev_meta, sizeof(int)*((V+1))); })
  .wait();

  // CSR END
  //LAUNCH CONFIG
  int NUM_THREADS = 1048576;
  int stride = NUM_THREADS;

  // TIMER START
  std::chrono::steady_clock::time_point tic = std::chrono::steady_clock::now();

  //DECLAR DEVICE AND HOST vars in params

  //BEGIN DSL PARSING 
  int* d_cf;
  d_cf=malloc_device<int>(E, Q);

  int* d_h;
  d_h=malloc_device<int>(V, Q);

  int* d_e;
  d_e=malloc_device<int>(V, Q);

  int ExcessTotal = 0; // asst in main

  int N = g.num_nodes( ); // asst in main

  int *d_s = malloc_device<int>(1, Q);
  Q.submit([&](handler &h)
    { h.memcpy(d_s, &s, 1 * sizeof(int)); })
  .wait();

  int *d_N = malloc_device<int>(1, Q);
  Q.submit([&](handler &h)
    { h.memcpy(d_N, &N, 1 * sizeof(int)); })
  .wait();

  Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> v){for (; v < V; v += NUM_THREADS){ // BEGIN KER FUN via ADDKERNEL
    if (v == s){ // if filter begin 
      d_h[v] = N;
      d_e[v] = 0;

    } // if filter end
    else
    d_h[v] = 0;
    d_e[v] = 0;

    // for all else part
    for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
      int nbr = d_data[edge];
      int e1 = edge;
      d_cf[e1] = d_weight[e1];
      if (d_cf[e1] == -1){ // if filter begin 
        d_cf_backward[e1] = 0;

      } // if filter end

    } //  end FOR NBR ITR. TMP FIX!
  }
}); }).wait(); // end KER FUNC

Q.submit([&](handler &h)
{ h.memcpy(&s, d_s, 1 * sizeof(int)); })
.wait();

Q.submit([&](handler &h)
{ h.memcpy(&N, d_N, 1 * sizeof(int)); })
.wait();

int v = 0; // asst in main

do{
if (v == s){ // if filter begin 
  d_h[v] = N;
  d_e[v] = 0;

} // if filter end
else
d_h[v] = 0;
d_e[v] = 0;

// for all else part
for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
  int nbr = d_data[edge];
  int e1 = edge;
  d_cf[e1] = weight[e1];
  if (cf[e1] == -1){ // if filter begin 
    d_cf_backward[e1] = 0;

  } // if filter end

} //  end FOR NBR ITR. TMP FIX!
v = v + 1;

}while(v < N);
bool finished = false; // asst in main

do{
int *d_v = malloc_device<int>(1, Q);
Q.submit([&](handler &h)
  { h.memcpy(d_v, &v, 1 * sizeof(int)); })
.wait();

Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> v){for (; v < V; v += NUM_THREADS){ // BEGIN KER FUN via ADDKERNEL
  int cycle = 100; // asst in main

  do{
    int e1 = d_e[v]; // asst in main

    int h1 = INT_MAX; // asst in main

    int v1 = -1; // asst in main

    // for all else part
    for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
      int nbr = d_data[edge];
      int h2 = d_h[nbr]; // asst in main

      if (h2 < h1){ // if filter begin 
        v1 = nbr;
        h1 = h2;

      } // if filter end

    } //  end FOR NBR ITR. TMP FIX!
    if (d_h[v] > h1){ // if filter begin 
      int ee = edge;
      int d = -1; // asst in main

      if (e1 < d_cf_forward[ee]){ // if filter begin 
        d = e1;

      } // if filter end
      else
      d = d_cf_forward[ee];

      d_cf_backward[ee] = d_cf_backward[ee]- d;

    } // if filter end

  }while(cycle > 0);
}
}); }).wait(); // end KER FUNC

Q.submit([&](handler &h)
{ h.memcpy(&v, d_v, 1 * sizeof(int)); })
.wait();


}while(finished);

//free up!! all propVars in this BLOCK!
free(d_e, Q);
free(d_h, Q);
free(d_cf, Q);

//TIMER STOP
std::chrono::steady_clock::time_point toc = std::chrono::steady_clock::now();
std::cout<<"Time required: "<<std::chrono::duration_cast<std::chrono::microseconds>(toc - tic).count()<<"[µs]"<<std::endl;

} //end FUN

#endif
