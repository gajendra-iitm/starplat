// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#include "Mod-OP-Test.h"

void tst(int a,int b)

{

  //DECLAR DEVICE AND HOST vars in params

  //BEGIN DSL PARSING 
  int c = a % b; // asst in .cu


  //TIMER STOP
  cudaEventRecord(stop,0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&milliseconds, start, stop);
  printf("GPU Time: %.6f ms\n", milliseconds);

} //end FUN
