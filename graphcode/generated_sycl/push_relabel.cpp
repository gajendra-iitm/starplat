#ifndef GENCPP_PUSH_RELABEL_H
#define GENCPP_PUSH_RELABEL_H
#include <CL/sycl.hpp>
#include <iostream>
#include <fstream>
#include "../graph.hpp"
using namespace sycl;

void do_max_flow(graph& g,int source,int sink,int kernel_parameter
)
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
  int* d_label;
  d_label=malloc_device<int>(V, Q);

  Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> i){
    for (; i < V; i += stride) d_label[i] = (int)0;});
}).wait();

Q.submit([&](handler &h){ h.single_task([=](){
  d_label[source] = (int)g.num_nodes( );});
}).wait(); //InitIndexDevice

int* d_excess;
d_excess=malloc_device<int>(V, Q);

Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> i){
for (; i < V; i += stride) d_excess[i] = (int)0;});
}).wait();

int* d_residual_capacity;
d_residual_capacity=malloc_device<int>(E, Q);

Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> i){
for (; i < E; i += stride) d_residual_capacity[i] = (int)g.getEdgeLen( );});
}).wait();

int excesstotal = 0; // asst in main

int *d_source = malloc_device<int>(1, Q);
Q.submit([&](handler &h)
{ h.memcpy(d_source, &source, 1 * sizeof(int)); })
.wait();

int *d_excesstotal = malloc_device<int>(1, Q);
Q.submit([&](handler &h)
{ h.memcpy(d_excesstotal, &excesstotal, 1 * sizeof(int)); })
.wait();

Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> v){for (; v < V; v += NUM_THREADS){ // BEGIN KER FUN via ADDKERNEL
if (v == source){ // if filter begin 
// for all else part
for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
int vv = d_data[edge];
int forward_edge = edge;
int backward_edge = edge;
int d = d_residual_capacity[forward_edge]; // asst in main

atomic_ref<int, memory_order::relaxed, memory_scope::device, access::address_space::global_space> atomic_data(d_excesstotal[0]);
atomic_data += d;
d_excess[vv] = d_excess[vv]+ d;
d_residual_capacity[forward_edge] = d_residual_capacity[forward_edge]- d;
d_residual_capacity[backward_edge] = d_residual_capacity[backward_edge]+ d;

} //  end FOR NBR ITR. TMP FIX!

} // if filter end
}
}); }).wait(); // end KER FUNC

Q.submit([&](handler &h)
{ h.memcpy(&source, d_source, 1 * sizeof(int)); })
.wait();

Q.submit([&](handler &h)
{ h.memcpy(&excesstotal, d_excesstotal, 1 * sizeof(int)); })
.wait();

bool flag = true; // asst in main

do{
int *d_source = malloc_device<int>(1, Q);
Q.submit([&](handler &h)
{ h.memcpy(d_source, &source, 1 * sizeof(int)); })
.wait();

int *d_sink = malloc_device<int>(1, Q);
Q.submit([&](handler &h)
{ h.memcpy(d_sink, &sink, 1 * sizeof(int)); })
.wait();

int *d_kernel_parameter = malloc_device<int>(1, Q);
Q.submit([&](handler &h)
{ h.memcpy(d_kernel_parameter, &kernel_parameter, 1 * sizeof(int)); })
.wait();

Q.submit([&](handler &h){ h.parallel_for(NUM_THREADS, [=](id<1> v){for (; v < V; v += NUM_THREADS){ // BEGIN KER FUN via ADDKERNEL
if (d_excess[v] > 0 && v != source && v != sink){ // if filter begin 
int x = kernel_parameter; // asst in main

do{
x = x + -1;
int hh = INT_MAX; // asst in main

int lv = -1; // asst in main

// for all else part
for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
int vv = d_data[edge];
int e = edge;
int xxx = d_residual_capacity[e]; // asst in main

if (d_label[vv] < hh && xxx > 0){ // if filter begin 
  lv = vv;
  hh = d_label[vv];

} // if filter end

} //  end FOR NBR ITR. TMP FIX!
if (d_label[v] > hh && lv != -1){ // if filter begin 
int current_edge = edge;
int cec = d_residual_capacity[current_edge]; // asst in main

if (d_excess[v] > 0 && cec > 0){ // if filter begin 
  int forward_edge = edge;
  int backward_edge = edge;
  int fec = d_residual_capacity[forward_edge]; // asst in main

  int bec = d_residual_capacity[backward_edge]; // asst in main

  int d = fec; // asst in main

  if (d_excess[v] < fec){ // if filter begin 
    d = d_excess[v];

  } // if filter end
  d_excess[v] = d_excess[v]- d;
  d_excess[lv] = d_excess[lv]+ d;
  d_residual_capacity[forward_edge] = d_residual_capacity[forward_edge]- d;
  d_residual_capacity[backward_edge] = d_residual_capacity[backward_edge]+ d;

} // if filter end

} // if filter end
else
if (lv != -1){ // if filter begin 
d_label[v] = hh + 1;

} // if filter end

}while(x > 0);

} // if filter end
}
}); }).wait(); // end KER FUNC

Q.submit([&](handler &h)
{ h.memcpy(&source, d_source, 1 * sizeof(int)); })
.wait();

Q.submit([&](handler &h)
{ h.memcpy(&sink, d_sink, 1 * sizeof(int)); })
.wait();

Q.submit([&](handler &h)
{ h.memcpy(&kernel_parameter, d_kernel_parameter, 1 * sizeof(int)); })
.wait();

if (excess[source] + excess[sink] == excesstotal){ // if filter begin 
flag = false;

} // if filter end

}while(flag);

//free up!! all propVars in this BLOCK!
free(d_residual_capacity, Q);
free(d_excess, Q);
free(d_label, Q);

//TIMER STOP
std::chrono::steady_clock::time_point toc = std::chrono::steady_clock::now();
std::cout<<"Time required: "<<std::chrono::duration_cast<std::chrono::microseconds>(toc - tic).count()<<"[µs]"<<std::endl;

} //end FUN

#endif
