// FOR BC: nvcc bc_dsl_v2.cu -arch=sm_60 -std=c++14 -rdc=true # HW must support CC 6.0+ Pascal or after
#include "ParamByRef_test.h"

void test(double &arg1,int &arg2)

{

  //DECLAR DEVICE AND HOST vars in params

  //BEGIN DSL PARSING 
  int a = 0; // asst in .cu

  if (a == 0){ // if filter begin 
    arg1 = 1.000000;
    arg2 = 2;

  } // if filter end
  else
  arg1 = 2.000000;
  arg2 = 3;


  //TIMER STOP
  cudaEventRecord(stop,0);
  cudaEventSynchronize(stop);
  cudaEventElapsedTime(&milliseconds, start, stop);
  printf("GPU Time: %.6f ms\n", milliseconds);

} //end FUN
