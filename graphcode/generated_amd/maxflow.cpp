#include "maxflow.h"

void maxflow(graph& g,int s,int t)
{
  //Getting platforms
  cl_int status;
  cl_platform_id *platforms = NULL;
  cl_uint number_of_platforms;
  status = clGetPlatformIDs(0, NULL, &number_of_platforms);
  platforms = (cl_platform_id *)malloc(number_of_platforms*sizeof(cl_platform_id));
  status = clGetPlatformIDs(number_of_platforms, platforms, NULL);
  printf("Got Platform, status = %d \n", status);

  //Getting Devices present on platform
  cl_device_id *devices= NULL;
  cl_uint number_of_devices;
  status = clGetDeviceIDs(platforms[0],CL_DEVICE_TYPE_GPU, 0, NULL, &number_of_devices);
  devices = (cl_device_id *)malloc(number_of_devices*sizeof(cl_device_id));
  status = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, number_of_devices, devices, NULL);
  printf("Got Devices, status = %d \n", status);

  //Creating context
  cl_context context;
  context = clCreateContext(NULL, number_of_devices, devices, NULL, NULL, &status);

  //Creating command queue
  cl_command_queue command_queue ;
  command_queue = clCreateCommandQueue(context, devices[0], CL_QUEUE_PROFILING_ENABLE , &status);
  printf("command queue created, status = %d\n", status);

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


  cl_mem d_meta = clCreateBuffer(context, CL_MEM_READ_WRITE, (1+V)*sizeof(int), NULL, &status);
  cl_mem d_data = clCreateBuffer(context, CL_MEM_READ_WRITE, (E)*sizeof(int), NULL, &status);
  cl_mem d_src = clCreateBuffer(context, CL_MEM_READ_WRITE, (E)*sizeof(int), NULL, &status);
  cl_mem d_weight = clCreateBuffer(context, CL_MEM_READ_WRITE, (E)*sizeof(int), NULL, &status);
  cl_mem d_rev_meta = clCreateBuffer(context, CL_MEM_READ_WRITE, (V+1)*sizeof(int), NULL, &status);
  cl_mem d_modified_next = clCreateBuffer(context, CL_MEM_READ_WRITE, (V)*sizeof(int), NULL, &status);

  status = clEnqueueWriteBuffer(command_queue,   d_meta , CL_TRUE, 0, sizeof(int)*(V+1),   h_meta, 0, NULL, NULL );
  status = clEnqueueWriteBuffer(command_queue,   d_data , CL_TRUE, 0, sizeof(int)*E,   h_data, 0, NULL, NULL );
  status = clEnqueueWriteBuffer(command_queue,    d_src , CL_TRUE, 0, sizeof(int)*E,    h_src, 0, NULL, NULL );
  status = clEnqueueWriteBuffer(command_queue, d_weight , CL_TRUE, 0, sizeof(int)*E, h_weight, 0, NULL, NULL );
  status = clEnqueueWriteBuffer(command_queue, d_rev_meta , CL_TRUE, 0, sizeof(int)*(V+1), h_rev_meta, 0, NULL, NULL );

  // CSR END

  // TIMER START
  cl_event event;
  double kernelTime ;
  double totalTime = 0.0;
  cl_ulong start, end;
  cl_ulong convertToMS = 1e6;

  //DECLAR DEVICE AND HOST vars in params
  int *h_s;
  cl_mem d_s = clCreateBuffer(context,CL_MEM_READ_WRITE,sizeof(int),NULL, &status);

  int *h_t;
  cl_mem d_t = clCreateBuffer(context,CL_MEM_READ_WRITE,sizeof(int),NULL, &status);


  //BEGIN DSL PARSING 
  int *h_cf = (int *)malloc(E*sizeof(int));
  cl_mem d_cf = clCreateBuffer(context,CL_MEM_READ_WRITE,(E)*sizeof(int),NULL, &status);

  int *h_h = (int *)malloc(V*sizeof(int));
  cl_mem d_h = clCreateBuffer(context,CL_MEM_READ_WRITE,(V)*sizeof(int),NULL, &status);

  int *h_e = (int *)malloc(V*sizeof(int));
  cl_mem d_e = clCreateBuffer(context,CL_MEM_READ_WRITE,(V)*sizeof(int),NULL, &status);

  int *h_ExcessTotal ;
  cl_mem d_ExcessTotal= clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int), NULL, &status);

  int ExcessTotal = 0; 
  int *h_N ;
  cl_mem d_N= clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int), NULL, &status);

  int N = g.num_nodes( ); 

  //ForAll started here

  status = clEnqueueWriteBuffer(command_queue, d_s, CL_TRUE, 0, sizeof(int),&s , 0, NULL, NULL);

  status = clEnqueueWriteBuffer(command_queue, d_N, CL_TRUE, 0, sizeof(int),&N , 0, NULL, NULL);


  //Reading kernel file
  FILE* kernelfp = fopen("maxflow.cl", "rb"); 
  size_t program_size;
  fseek(kernelfp, 0, SEEK_END);
  program_size = ftell(kernelfp);
  rewind(kernelfp);
  char* kernelSource;
  kernelSource = (char *)malloc((program_size+1)* sizeof(char));
  fread(kernelSource, sizeof(char), program_size, kernelfp);
  kernelSource[program_size] = (char)NULL ;
  fclose(kernelfp);

  //Creating program from source(Create and build Program)
  cl_program program = clCreateProgramWithSource(context, 1, (const char **)&kernelSource, NULL, &status);
  printf("Progran created from source, status = %d \n", status);
  status = clBuildProgram(program, number_of_devices, devices, " -I ./", NULL, NULL);
  printf(" Program building completed, status = %d \n ",status);

  //Variable for launch configuration
  size_t global_size;
  size_t local_size;
  cl_kernel maxflow = clCreateKernel(program, "maxflow_kernel", &status);
  if(status != CL_SUCCESS){
    printf("Failed to create maxflow kernel.\n");
    return ;
  }
  status = clSetKernelArg(maxflow,0, sizeof(int),(void *)&V);
  status = clSetKernelArg(maxflow,1, sizeof(int),(void *)&E);
  status = clSetKernelArg(maxflow,2, sizeof(cl_mem),(void *)&d_meta);
  status = clSetKernelArg(maxflow,3, sizeof(cl_mem),(void *)&d_data);
  status = clSetKernelArg(maxflow,4, sizeof(cl_mem),(void *)&d_src);
  status = clSetKernelArg(maxflow,5, sizeof(cl_mem),(void *)&d_weight);
  status = clSetKernelArg(maxflow,6, sizeof(cl_mem),(void *)&d_rev_meta);
  status = clSetKernelArg(maxflow, 7, sizeof(cl_mem), (void *)&d_cf);
  status = clSetKernelArg(maxflow, 8, sizeof(cl_mem), (void *)&d_s);
  status = clSetKernelArg(maxflow, 9, sizeof(cl_mem), (void *)&d_N);
  status = clSetKernelArg(maxflow, 10, sizeof(cl_mem), (void *)&d_e);
  status = clSetKernelArg(maxflow, 11, sizeof(cl_mem), (void *)&d_h);

  local_size = 128;
  global_size = (V + local_size -1)/ local_size * local_size;

  status = clEnqueueNDRangeKernel(command_queue,maxflow, 1,NULL, &global_size, &local_size , 0,NULL,&event);
  clWaitForEvents(1,&event);
  status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);
  status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);
  kernelTime = (double)(end-start)/convertToMS;
  totalTime = totalTime+ kernelTime;

  status = clReleaseKernel(maxflow);

  status = clEnqueueReadBuffer(command_queue, d_s, CL_TRUE, 0, sizeof(int),&s , 0, NULL, NULL);

  status = clEnqueueReadBuffer(command_queue, d_N, CL_TRUE, 0, sizeof(int),&N , 0, NULL, NULL);
  int *h_v ;
  cl_mem d_v= clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int), NULL, &status);

  int v = 0; 
  do{
    if (v == s){ // if filter begin 
      d_h[v] = N;
      d_e[v] = 0;

    } // if filter end
    else
    d_h[v] = 0;
    d_e[v] = 0;


    //ForAll started here
    int e1 = edge; d_cf[e1] = weight[e1];
    if (cf[e1] == -1){ // if filter begin 
      d_cf_backward[e1] = 0;

    } // if filter end

  } //  end FOR NBR ITR. TMP FIX!
  v = v + 1;

}while(v < N);
int *h_finished ;
cl_mem d_finished= clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int), NULL, &status);

int finished = false; 
do{

  //ForAll started here

  status = clEnqueueWriteBuffer(command_queue, d_v, CL_TRUE, 0, sizeof(int),&v , 0, NULL, NULL);

  cl_kernel maxflow = clCreateKernel(program, "maxflow_kernel", &status);
  if(status != CL_SUCCESS){
    printf("Failed to create maxflow kernel.\n");
    return ;
  }
  status = clSetKernelArg(maxflow,0, sizeof(int),(void *)&V);
  status = clSetKernelArg(maxflow,1, sizeof(int),(void *)&E);
  status = clSetKernelArg(maxflow,2, sizeof(cl_mem),(void *)&d_meta);
  status = clSetKernelArg(maxflow,3, sizeof(cl_mem),(void *)&d_data);
  status = clSetKernelArg(maxflow,4, sizeof(cl_mem),(void *)&d_src);
  status = clSetKernelArg(maxflow,5, sizeof(cl_mem),(void *)&d_weight);
  status = clSetKernelArg(maxflow,6, sizeof(cl_mem),(void *)&d_rev_meta);
  status = clSetKernelArg(maxflow, 7, sizeof(cl_mem), (void *)&d_h);
  status = clSetKernelArg(maxflow, 8, sizeof(cl_mem), (void *)&d_v);
  status = clSetKernelArg(maxflow, 9, sizeof(cl_mem), (void *)&d_e);

  local_size = 128;
  global_size = (V + local_size -1)/ local_size * local_size;

  status = clEnqueueNDRangeKernel(command_queue,maxflow, 1,NULL, &global_size, &local_size , 0,NULL,&event);
  clWaitForEvents(1,&event);
  status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);
  status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);
  kernelTime = (double)(end-start)/convertToMS;
  totalTime = totalTime+ kernelTime;

  status = clReleaseKernel(maxflow);

  status = clEnqueueReadBuffer(command_queue, d_v, CL_TRUE, 0, sizeof(int),&v , 0, NULL, NULL);







}while(finished);

//Free up!! all propVars in this BLOCK!
status = clReleaseMemObject(d_e);
free(h_e);
status = clReleaseMemObject(d_h);
free(h_h);

//TIMER STOP
printf("Total Kernel time = %0.2lf ms.\n ", totalTime);

//Release openCL objects
printf("Started releasing Objects\n");
status = clReleaseMemObject(d_meta);
status = clReleaseMemObject(d_data);
status = clReleaseMemObject(d_src);
status = clReleaseMemObject(d_weight);
status = clReleaseMemObject(d_rev_meta);
status = clReleaseMemObject(d_modified_next);
status = clReleaseProgram(program);
status = clFlush(command_queue);
status = clFinish(command_queue);
status = clReleaseCommandQueue(command_queue);
status = clReleaseContext(context);
free(h_meta);
free(h_data);
free(h_src);
free(h_weight);
free(h_rev_meta);
} //end FUN
