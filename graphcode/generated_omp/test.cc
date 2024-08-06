#include"test.h"

void test1(graph& g , float* features , int* labels)
{
  int num_epoch = 100;
  int num_layer = 5;
  vector<int> layers;
  layers.pushback(num_features);

  layers.pushback(16);

  layers.pushback(num_classes);

  int x = 1;
  initialize_layers(layers,Xavier_transform);

  bool flag = true;
  bool flag1 = true;
  while (flag1){
    x = 0;
    #pragma omp parallel for
    for (int nod = 0; nod < g.num_nodes(); nod ++) 
    {
      x = x + 1;
      for (int edge = g.indexofNodes[nod]; edge < g.indexofNodes[nod+1]; edge ++) 
      {int n = g.edgeList[edge] ;
        if (labels[n] == labels[nod] )
          {
          int num_neurons = layers.getIdx(x);
          message_pass(nod,n,features,"sum",num_neurons,1);

        }
      }
    }
  }

}
