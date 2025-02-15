#include "push_relabel.h"

void do_max_flow(graph& g,int source,int sink,int kernel_parameter
)
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
  int *h_source;
  cl_mem d_source = clCreateBuffer(context,CL_MEM_READ_WRITE,sizeof(int),NULL, &status);

  int *h_sink;
  cl_mem d_sink = clCreateBuffer(context,CL_MEM_READ_WRITE,sizeof(int),NULL, &status);

  int *h_kernel_parameter;
  cl_mem d_kernel_parameter = clCreateBuffer(context,CL_MEM_READ_WRITE,sizeof(int),NULL, &status);


  //BEGIN DSL PARSING 
  int *h_label = (int *)malloc(V*sizeof(int));
  cl_mem d_label = clCreateBuffer(context,CL_MEM_READ_WRITE,(V)*sizeof(int),NULL, &status);


  //Reading kernel file
  FILE* kernelfp = fopen("push_relabel.cl", "rb"); 
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
  // Creating initlabel_kernel  Kernel
  cl_kernel initlabel_kernel = clCreateKernel(program, "initlabel_kernel", &status);

  // Initialization for label variable
  int labelValue = (int)0; 
  status = clSetKernelArg(initlabel_kernel, 0 , sizeof(cl_mem), (void *)& d_label);
  status = clSetKernelArg(initlabel_kernel, 1, sizeof(int) , (void*)& labelValue);
  status = clSetKernelArg(initlabel_kernel, 2, sizeof(int), (void*)&V);
  local_size = 128;
  global_size = (V + local_size -1)/ local_size * local_size;
  status = clEnqueueNDRangeKernel(command_queue, initlabel_kernel, 1, NULL, &global_size, &local_size, 0,NULL,&event);

  clWaitForEvents(1,&event);
  status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);
  status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);
  kernelTime = (double)(end-start)/convertToMS;
  totalTime = totalTime+ kernelTime;
  status = clReleaseKernel(initlabel_kernel);

  cl_kernel initIndexlabel_kernel = clCreateKernel(program, "initIndexlabel_kernel", &status);
  //Indexlabel source initialization
  int initlabelvalsource = g.num_nodes( );
  status = clSetKernelArg(initIndexlabel_kernel , 0,sizeof(cl_mem) ,(void *)&d_label);
  status = clSetKernelArg(initIndexlabel_kernel, 1,sizeof(int), (void*)&source);
  status = clSetKernelArg(initIndexlabel_kernel, 2, sizeof(int),(void *)&initlabelvalsource);
  global_size = local_size = 1;
  status  = clEnqueueNDRangeKernel(command_queue, initIndexlabel_kernel,1,NULL, &global_size, &local_size, 0, NULL, &event);
  clWaitForEvents(1,&event);
  status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);
  status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);
  kernelTime = (double)(end-start)/convertToMS;
  totalTime = totalTime+ kernelTime;

  status = clReleaseKernel(initIndexlabel_kernel);

  int *h_excess = (int *)malloc(V*sizeof(int));
  cl_mem d_excess = clCreateBuffer(context,CL_MEM_READ_WRITE,(V)*sizeof(int),NULL, &status);

  // Creating initexcess_kernel  Kernel
  cl_kernel initexcess_kernel = clCreateKernel(program, "initexcess_kernel", &status);

  // Initialization for excess variable
  int excessValue = (int)0; 
  status = clSetKernelArg(initexcess_kernel, 0 , sizeof(cl_mem), (void *)& d_excess);
  status = clSetKernelArg(initexcess_kernel, 1, sizeof(int) , (void*)& excessValue);
  status = clSetKernelArg(initexcess_kernel, 2, sizeof(int), (void*)&V);
  local_size = 128;
  global_size = (V + local_size -1)/ local_size * local_size;
  status = clEnqueueNDRangeKernel(command_queue, initexcess_kernel, 1, NULL, &global_size, &local_size, 0,NULL,&event);

  clWaitForEvents(1,&event);
  status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);
  status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);
  kernelTime = (double)(end-start)/convertToMS;
  totalTime = totalTime+ kernelTime;
  status = clReleaseKernel(initexcess_kernel);

  int *h_residual_capacity = (int *)malloc(E*sizeof(int));
  cl_mem d_residual_capacity = clCreateBuffer(context,CL_MEM_READ_WRITE,(E)*sizeof(int),NULL, &status);

  int *h_excesstotal ;
  cl_mem d_excesstotal= clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int), NULL, &status);

  int excesstotal = 0; 

  //ForAll started here

  status = clEnqueueWriteBuffer(command_queue, d_source, CL_TRUE, 0, sizeof(int),&source , 0, NULL, NULL);

  status = clEnqueueWriteBuffer(command_queue, d_excesstotal, CL_TRUE, 0, sizeof(int),&excesstotal , 0, NULL, NULL);

  cl_kernel do_max_flow = clCreateKernel(program, "do_max_flow_kernel", &status);
  if(status != CL_SUCCESS){
    printf("Failed to create do_max_flow kernel.\n");
    return ;
  }
  status = clSetKernelArg(do_max_flow,0, sizeof(int),(void *)&V);
  status = clSetKernelArg(do_max_flow,1, sizeof(int),(void *)&E);
  status = clSetKernelArg(do_max_flow,2, sizeof(cl_mem),(void *)&d_meta);
  status = clSetKernelArg(do_max_flow,3, sizeof(cl_mem),(void *)&d_data);
  status = clSetKernelArg(do_max_flow,4, sizeof(cl_mem),(void *)&d_src);
  status = clSetKernelArg(do_max_flow,5, sizeof(cl_mem),(void *)&d_weight);
  status = clSetKernelArg(do_max_flow,6, sizeof(cl_mem),(void *)&d_rev_meta);
  status = clSetKernelArg(do_max_flow, 7, sizeof(cl_mem), (void *)&d_source);
  status = clSetKernelArg(do_max_flow, 8, sizeof(cl_mem), (void *)&d_residual_capacity);
  status = clSetKernelArg(do_max_flow, 9, sizeof(cl_mem), (void *)&d_excesstotal);
  status = clSetKernelArg(do_max_flow, 10, sizeof(cl_mem), (void *)&d_excess);

  local_size = 128;
  global_size = (V + local_size -1)/ local_size * local_size;

  status = clEnqueueNDRangeKernel(command_queue,do_max_flow, 1,NULL, &global_size, &local_size , 0,NULL,&event);
  clWaitForEvents(1,&event);
  status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);
  status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);
  kernelTime = (double)(end-start)/convertToMS;
  totalTime = totalTime+ kernelTime;

  status = clReleaseKernel(do_max_flow);

  status = clEnqueueReadBuffer(command_queue, d_source, CL_TRUE, 0, sizeof(int),&source , 0, NULL, NULL);

  status = clEnqueueReadBuffer(command_queue, d_excesstotal, CL_TRUE, 0, sizeof(int),&excesstotal , 0, NULL, NULL);

  int *h_flag ;
  cl_mem d_flag= clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int), NULL, &status);

  int flag = true; 
  do{

    //ForAll started here

    status = clEnqueueWriteBuffer(command_queue, d_source, CL_TRUE, 0, sizeof(int),&source , 0, NULL, NULL);

    status = clEnqueueWriteBuffer(command_queue, d_sink, CL_TRUE, 0, sizeof(int),&sink , 0, NULL, NULL);

    status = clEnqueueWriteBuffer(command_queue, d_kernel_parameter, CL_TRUE, 0, sizeof(int),&kernel_parameter , 0, NULL, NULL);

    cl_kernel do_max_flow = clCreateKernel(program, "do_max_flow_kernel", &status);
    if(status != CL_SUCCESS){
      printf("Failed to create do_max_flow kernel.\n");
      return ;
    }
    status = clSetKernelArg(do_max_flow,0, sizeof(int),(void *)&V);
    status = clSetKernelArg(do_max_flow,1, sizeof(int),(void *)&E);
    status = clSetKernelArg(do_max_flow,2, sizeof(cl_mem),(void *)&d_meta);
    status = clSetKernelArg(do_max_flow,3, sizeof(cl_mem),(void *)&d_data);
    status = clSetKernelArg(do_max_flow,4, sizeof(cl_mem),(void *)&d_src);
    status = clSetKernelArg(do_max_flow,5, sizeof(cl_mem),(void *)&d_weight);
    status = clSetKernelArg(do_max_flow,6, sizeof(cl_mem),(void *)&d_rev_meta);
    status = clSetKernelArg(do_max_flow, 7, sizeof(cl_mem), (void *)&d_source);
    status = clSetKernelArg(do_max_flow, 8, sizeof(cl_mem), (void *)&d_sink);
    status = clSetKernelArg(do_max_flow, 9, sizeof(cl_mem), (void *)&d_excess);
    status = clSetKernelArg(do_max_flow, 10, sizeof(cl_mem), (void *)&d_residual_capacity);
    status = clSetKernelArg(do_max_flow, 11, sizeof(cl_mem), (void *)&d_label);
    status = clSetKernelArg(do_max_flow, 12, sizeof(cl_mem), (void *)&d_kernel_parameter);

    local_size = 128;
    global_size = (V + local_size -1)/ local_size * local_size;

    status = clEnqueueNDRangeKernel(command_queue,do_max_flow, 1,NULL, &global_size, &local_size , 0,NULL,&event);
    clWaitForEvents(1,&event);
    status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, NULL);
    status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, NULL);
    kernelTime = (double)(end-start)/convertToMS;
    totalTime = totalTime+ kernelTime;

    status = clReleaseKernel(do_max_flow);

    status = clEnqueueReadBuffer(command_queue, d_source, CL_TRUE, 0, sizeof(int),&source , 0, NULL, NULL);

    status = clEnqueueReadBuffer(command_queue, d_sink, CL_TRUE, 0, sizeof(int),&sink , 0, NULL, NULL);

    status = clEnqueueReadBuffer(command_queue, d_kernel_parameter, CL_TRUE, 0, sizeof(int),&kernel_parameter , 0, NULL, NULL);








    if (excess[source] + excess[sink] == excesstotal){ // if filter begin 
      flag = false;

    } // if filter end

  }while(flag);

  //Free up!! all propVars in this BLOCK!
  status = clReleaseMemObject(d_excess);
  free(h_excess);
  status = clReleaseMemObject(d_label);
  free(h_label);

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
