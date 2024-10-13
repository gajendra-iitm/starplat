#include "ParamByRef_test.h"

void test(double &arg1,int &arg2)
{

  //DECLAR DEVICE AND HOST vars in params
  double *h_arg1;
  cl_mem d_arg1 = clCreateBuffer(context,CL_MEM_READ_WRITE,sizeof(double),NULL, &status);

  int *h_arg2;
  cl_mem d_arg2 = clCreateBuffer(context,CL_MEM_READ_WRITE,sizeof(int),NULL, &status);


  //BEGIN DSL PARSING 
  int *h_a ;
  cl_mem d_a= clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int), NULL, &status);

  int a = 0; 
  if (a == 0){ // if filter begin 
    arg1 = 1.000000;
    arg2 = 2;

  } // if filter end
  else
  arg1 = 2.000000;
  arg2 = 3;


  //TIMER STOP
  printf("Total Kernel time = %0.2lf ms.\n ", totalTime);

  //Release openCL objects
  printf("Started releasing Objects\n");
  status = clReleaseProgram(program);
} //end FUN
