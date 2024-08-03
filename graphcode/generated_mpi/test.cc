#include"test.h"

void test1(Graph& g, NodeProperty<float>& features, NodeProperty<int>& labels, boost::mpi::communicator world )
{
  int num_epoch = 100;
  int num_layer = 5;
  Container<int> layers;
  layers.pushback(num_features);

  layers.pushback(16);

  layers.pushback(num_classes);

  int x = 1;
  initialize_layers(layers,Xavier_transform, world);

  bool flag = true;
  bool flag1 = true;
  while (flag1){
    x = 0;
    x = x + 1;
    world.barrier();
    for (int nod = g.start_node(); nod <= g.end_node(); nod ++) 
    {
      for (int n : g.getNeighbors(nod)) 
      {
        if (labels.getValue(n) == labels.getValue(nod) )
        {
          int num_neurons = layers.getIdx(x);
          message_pass(nod,n,features,"sum",num_neurons,1, world);

        }
      }


    }
    world.barrier();


  }

}
