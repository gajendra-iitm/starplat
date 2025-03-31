#include "convexHull.h"


auto extremePoints(
  std::vector<double>&  xCoord,std::vector<double>&  yCoord)
{




  std::vector<int> ans;
  int minIndex;  int maxIndex;  double minX = 100000000;  double maxX = -1;  double index = 0;  
  while (index < xCoord.size() ){
    double v = xCoord[index];  
    if (v < minX) {
      minX = v;
      minIndex = index;


    } 

    if (v > maxX) {
      maxX = v;
      maxIndex = index;


    } 
    index++;



  }
  ans.push_back(minIndex);

  ans.push_back(maxIndex);

  return ans;


}
auto innerPoint(
  std::vector<int>&  R,std::vector<double>&  xCoord,std::vector<double>&  yCoord)
{




  std::vector<double> point(2, 0);
  int z = 0;  int o = 1;  
  for(int i = 0 ; i < R.size() ; i++)
  {
    int v = R[i];

    point[z] = point[z] + xCoord[v];
    point[o] = point[o] + yCoord[v];



  }

  point[z] = point[0] / R.size();
  point[o] = point[1] / R.size();
  return point;


}
void getSign(
  std::vector<int>&  R,std::vector<int>&  sign,std::vector<double>&  point,std::vector<double>&  xCoord,
  std::vector<double>&  yCoord,std::vector<int>&  label,std::vector<int>&  L,std::vector<int>&  mark,
  int  maxLabel)
{




  int n = L.size();  int* d_L0;
  hipMalloc(&d_L0, sizeof(int) * L.size());
  hipMemcpy(d_L0, L.data(), L.size() * sizeof(int), hipMemcpyHostToDevice);
  int* d_mark0;
  hipMalloc(&d_mark0, sizeof(int) * mark.size());
  hipMemcpy(d_mark0, mark.data(), mark.size() * sizeof(int), hipMemcpyHostToDevice);
  int* d_R0;
  hipMalloc(&d_R0, sizeof(int) * R.size());
  hipMemcpy(d_R0, R.data(), R.size() * sizeof(int), hipMemcpyHostToDevice);
  int* d_sign0;
  hipMalloc(&d_sign0, sizeof(int) * sign.size());
  hipMemcpy(d_sign0, sign.data(), sign.size() * sizeof(int), hipMemcpyHostToDevice);
  double* d_xCoord0;
  hipMalloc(&d_xCoord0, sizeof(double) * xCoord.size());
  hipMemcpy(d_xCoord0, xCoord.data(), xCoord.size() * sizeof(double), hipMemcpyHostToDevice);
  double* d_yCoord0;
  hipMalloc(&d_yCoord0, sizeof(double) * yCoord.size());
  hipMemcpy(d_yCoord0, yCoord.data(), yCoord.size() * sizeof(double), hipMemcpyHostToDevice);
  double* d_point0;
  hipMalloc(&d_point0, sizeof(double) * point.size());
  hipMemcpy(d_point0, point.data(), point.size() * sizeof(double), hipMemcpyHostToDevice);
  int* d_label0;
  hipMalloc(&d_label0, sizeof(int) * label.size());
  hipMemcpy(d_label0, label.data(), label.size() * sizeof(int), hipMemcpyHostToDevice);

  int V0 = L.size();
  const unsigned threadsPerBlock0 = 64;
  const unsigned numBlocks0 = (V0 + threadsPerBlock0 - 1) / threadsPerBlock0;

  getSign_kernel0<<<numBlocks0, threadsPerBlock0>>>(V0, d_L0, d_mark0, d_R0, d_sign0, d_xCoord0, d_yCoord0, d_point0, maxLabel, d_label0);
  hipDeviceSynchronize();
  hipMemcpy(L.data(), d_L0, L.size() * sizeof(int), hipMemcpyDeviceToHost);
  hipFree(d_L0);
  hipMemcpy(mark.data(), d_mark0, mark.size() * sizeof(int), hipMemcpyDeviceToHost);
  hipFree(d_mark0);
  hipMemcpy(R.data(), d_R0, R.size() * sizeof(int), hipMemcpyDeviceToHost);
  hipFree(d_R0);
  hipMemcpy(sign.data(), d_sign0, sign.size() * sizeof(int), hipMemcpyDeviceToHost);
  hipFree(d_sign0);
  hipMemcpy(xCoord.data(), d_xCoord0, xCoord.size() * sizeof(double), hipMemcpyDeviceToHost);
  hipFree(d_xCoord0);
  hipMemcpy(yCoord.data(), d_yCoord0, yCoord.size() * sizeof(double), hipMemcpyDeviceToHost);
  hipFree(d_yCoord0);
  hipMemcpy(point.data(), d_point0, point.size() * sizeof(double), hipMemcpyDeviceToHost);
  hipFree(d_point0);
  hipMemcpy(label.data(), d_label0, label.size() * sizeof(int), hipMemcpyDeviceToHost);
  hipFree(d_label0);






}
void updateDistance(
  std::vector<int>&  label,std::vector<float>&  distance,std::vector<double>&  xCoord,std::vector<double>&  yCoord,
  std::vector<int>&  L,std::vector<int>&  R,std::vector<int>&  mark,std::vector<int>&  sign,
  int  maxLabel)
{




  int n = L.size();  int* d_L1;
  hipMalloc(&d_L1, sizeof(int) * L.size());
  hipMemcpy(d_L1, L.data(), L.size() * sizeof(int), hipMemcpyHostToDevice);
  int* d_mark1;
  hipMalloc(&d_mark1, sizeof(int) * mark.size());
  hipMemcpy(d_mark1, mark.data(), mark.size() * sizeof(int), hipMemcpyHostToDevice);
  float* d_distance1;
  hipMalloc(&d_distance1, sizeof(float) * distance.size());
  hipMemcpy(d_distance1, distance.data(), distance.size() * sizeof(float), hipMemcpyHostToDevice);
  int* d_sign1;
  hipMalloc(&d_sign1, sizeof(int) * sign.size());
  hipMemcpy(d_sign1, sign.data(), sign.size() * sizeof(int), hipMemcpyHostToDevice);
  double* d_yCoord1;
  hipMalloc(&d_yCoord1, sizeof(double) * yCoord.size());
  hipMemcpy(d_yCoord1, yCoord.data(), yCoord.size() * sizeof(double), hipMemcpyHostToDevice);
  double* d_xCoord1;
  hipMalloc(&d_xCoord1, sizeof(double) * xCoord.size());
  hipMemcpy(d_xCoord1, xCoord.data(), xCoord.size() * sizeof(double), hipMemcpyHostToDevice);
  int* d_R1;
  hipMalloc(&d_R1, sizeof(int) * R.size());
  hipMemcpy(d_R1, R.data(), R.size() * sizeof(int), hipMemcpyHostToDevice);
  int* d_label1;
  hipMalloc(&d_label1, sizeof(int) * label.size());
  hipMemcpy(d_label1, label.data(), label.size() * sizeof(int), hipMemcpyHostToDevice);

  int V1 = L.size();
  const unsigned threadsPerBlock1 = 64;
  const unsigned numBlocks1 = (V1 + threadsPerBlock1 - 1) / threadsPerBlock1;

  updateDistance_kernel1<<<numBlocks1, threadsPerBlock1>>>(V1, d_L1, d_mark1, d_distance1, d_sign1, d_yCoord1, maxLabel, d_xCoord1, d_R1, d_label1);
  hipDeviceSynchronize();
  hipMemcpy(L.data(), d_L1, L.size() * sizeof(int), hipMemcpyDeviceToHost);
  hipFree(d_L1);
  hipMemcpy(mark.data(), d_mark1, mark.size() * sizeof(int), hipMemcpyDeviceToHost);
  hipFree(d_mark1);
  hipMemcpy(distance.data(), d_distance1, distance.size() * sizeof(float), hipMemcpyDeviceToHost);
  hipFree(d_distance1);
  hipMemcpy(sign.data(), d_sign1, sign.size() * sizeof(int), hipMemcpyDeviceToHost);
  hipFree(d_sign1);
  hipMemcpy(yCoord.data(), d_yCoord1, yCoord.size() * sizeof(double), hipMemcpyDeviceToHost);
  hipFree(d_yCoord1);
  hipMemcpy(xCoord.data(), d_xCoord1, xCoord.size() * sizeof(double), hipMemcpyDeviceToHost);
  hipFree(d_xCoord1);
  hipMemcpy(R.data(), d_R1, R.size() * sizeof(int), hipMemcpyDeviceToHost);
  hipFree(d_R1);
  hipMemcpy(label.data(), d_label1, label.size() * sizeof(int), hipMemcpyDeviceToHost);
  hipFree(d_label1);








}
auto updateHull(
  std::vector<float>&  distance,std::vector<double>&  xCoord,std::vector<double>&  yCoord,std::vector<int>&  R,
  std::vector<int>&  label,std::vector<int>&  mark,std::vector<int>&  sign,std::vector<double>&  point,
  std::vector<int>&  L,int  maxLabel)
{




  int n = L.size();  int* d_R2;
  hipMalloc(&d_R2, sizeof(int) * R.size());
  hipMemcpy(d_R2, R.data(), R.size() * sizeof(int), hipMemcpyHostToDevice);

  int V2 = R.size();
  const unsigned threadsPerBlock2 = 64;
  const unsigned numBlocks2 = (V2 + threadsPerBlock2 - 1) / threadsPerBlock2;

  updateHull_kernel2<<<numBlocks2, threadsPerBlock2>>>(V2, n, d_R2);
  hipDeviceSynchronize();
  hipMemcpy(R.data(), d_R2, R.size() * sizeof(int), hipMemcpyDeviceToHost);
  hipFree(d_R2);

  int index = 0;  int newVertices = 0;  
  for(int i = 0 ; i < distance.size() ; i++)
  {
    int v = distance[i];


    if (v > 0 && mark[index] == 0) {
      int tempIndex = label[index] + 2 * n;  
      if (R[tempIndex] == -1) {
        R[tempIndex] = index;
        newVertices++;


      }  else {
        int currMinIndex = R[tempIndex];  
        if (distance[currMinIndex] < v) {
          R[tempIndex] = index;


        } 


      }


    }  else {
      mark[index] = 1;


    }
    index++;



  }

  std::vector<int> newR(3 * n, -1);
  int* d_newR3;
  hipMalloc(&d_newR3, sizeof(int) * newR.size());
  hipMemcpy(d_newR3, newR.data(), newR.size() * sizeof(int), hipMemcpyHostToDevice);
  int* d_R3;
  hipMalloc(&d_R3, sizeof(int) * R.size());
  hipMemcpy(d_R3, R.data(), R.size() * sizeof(int), hipMemcpyHostToDevice);

  int V3 = newR.size();
  const unsigned threadsPerBlock3 = 64;
  const unsigned numBlocks3 = (V3 + threadsPerBlock3 - 1) / threadsPerBlock3;

  updateHull_kernel3<<<numBlocks3, threadsPerBlock3>>>(V3, d_newR3, d_R3, n);
  hipDeviceSynchronize();
  hipMemcpy(newR.data(), d_newR3, newR.size() * sizeof(int), hipMemcpyDeviceToHost);
  hipFree(d_newR3);
  hipMemcpy(R.data(), d_R3, R.size() * sizeof(int), hipMemcpyDeviceToHost);
  hipFree(d_R3);



  if (newVertices > 0) {
    int* d_R4;
    hipMalloc(&d_R4, sizeof(int) * R.size());
    hipMemcpy(d_R4, R.data(), R.size() * sizeof(int), hipMemcpyHostToDevice);
    int* d_newR4;
    hipMalloc(&d_newR4, sizeof(int) * newR.size());
    hipMemcpy(d_newR4, newR.data(), newR.size() * sizeof(int), hipMemcpyHostToDevice);

    int V4 = R.size();
    const unsigned threadsPerBlock4 = 64;
    const unsigned numBlocks4 = (V4 + threadsPerBlock4 - 1) / threadsPerBlock4;

    updateHull_kernel4<<<numBlocks4, threadsPerBlock4>>>(V4, d_R4, d_newR4);
    hipDeviceSynchronize();
    hipMemcpy(R.data(), d_R4, R.size() * sizeof(int), hipMemcpyDeviceToHost);
    hipFree(d_R4);
    hipMemcpy(newR.data(), d_newR4, newR.size() * sizeof(int), hipMemcpyDeviceToHost);
    hipFree(d_newR4);


  } 
  return newVertices;


}
void updateLabel(
  std::vector<int>&  label,std::vector<int>&  R,std::vector<double>&  xCoord,std::vector<double>&  yCoord,
  std::vector<int>&  mark,int  currHullSize)
{




  int n = label.size();  int* d_mark5;
  hipMalloc(&d_mark5, sizeof(int) * mark.size());
  hipMemcpy(d_mark5, mark.data(), mark.size() * sizeof(int), hipMemcpyHostToDevice);
  double* d_yCoord5;
  hipMalloc(&d_yCoord5, sizeof(double) * yCoord.size());
  hipMemcpy(d_yCoord5, yCoord.data(), yCoord.size() * sizeof(double), hipMemcpyHostToDevice);
  double* d_xCoord5;
  hipMalloc(&d_xCoord5, sizeof(double) * xCoord.size());
  hipMemcpy(d_xCoord5, xCoord.data(), xCoord.size() * sizeof(double), hipMemcpyHostToDevice);
  int* d_R5;
  hipMalloc(&d_R5, sizeof(int) * R.size());
  hipMemcpy(d_R5, R.data(), R.size() * sizeof(int), hipMemcpyHostToDevice);
  int* d_label5;
  hipMalloc(&d_label5, sizeof(int) * label.size());
  hipMemcpy(d_label5, label.data(), label.size() * sizeof(int), hipMemcpyHostToDevice);

  int V5 = label.size();
  const unsigned threadsPerBlock5 = 64;
  const unsigned numBlocks5 = (V5 + threadsPerBlock5 - 1) / threadsPerBlock5;

  updateLabel_kernel5<<<numBlocks5, threadsPerBlock5>>>(V5, d_mark5, d_yCoord5, d_xCoord5, d_R5, currHullSize, d_label5);
  hipDeviceSynchronize();
  hipMemcpy(mark.data(), d_mark5, mark.size() * sizeof(int), hipMemcpyDeviceToHost);
  hipFree(d_mark5);
  hipMemcpy(yCoord.data(), d_yCoord5, yCoord.size() * sizeof(double), hipMemcpyDeviceToHost);
  hipFree(d_yCoord5);
  hipMemcpy(xCoord.data(), d_xCoord5, xCoord.size() * sizeof(double), hipMemcpyDeviceToHost);
  hipFree(d_xCoord5);
  hipMemcpy(R.data(), d_R5, R.size() * sizeof(int), hipMemcpyDeviceToHost);
  hipFree(d_R5);
  hipMemcpy(label.data(), d_label5, label.size() * sizeof(int), hipMemcpyDeviceToHost);
  hipFree(d_label5);






}
auto convexHull(
  int  n,std::vector<double>&  xCoord,std::vector<double>&  yCoord)
{




  std::vector<int> label(n, 0);
  std::vector<float> distance(n);
  std::vector<int> L(n);
  std::vector<int> R(3 * n, -1);
  std::vector<int> mark(n, 0);
  std::vector<double> point(2, 0);
  std::vector<int> sign(2 * n, 0);
  int A;  int B;  std::vector<int> extreme;
  extreme = extremePoints(xCoord,yCoord);
  A = extreme[0];
  B = extreme[1];
  int* d_L6;
  hipMalloc(&d_L6, sizeof(int) * L.size());
  hipMemcpy(d_L6, L.data(), L.size() * sizeof(int), hipMemcpyHostToDevice);

  int V6 = L.size();
  const unsigned threadsPerBlock6 = 64;
  const unsigned numBlocks6 = (V6 + threadsPerBlock6 - 1) / threadsPerBlock6;

  convexHull_kernel6<<<numBlocks6, threadsPerBlock6>>>(V6, d_L6);
  hipDeviceSynchronize();
  hipMemcpy(L.data(), d_L6, L.size() * sizeof(int), hipMemcpyDeviceToHost);
  hipFree(d_L6);
  R[0] = A;
  R[1] = B;
  mark[A] = 1;
  mark[B] = 1;
  updateDistance(label,distance,xCoord,yCoord,L,R,mark,sign,2);

  int upperHull = 0;  float upperDist = 0;  int lowerHull = -1;  float lowerDist = -1;  
  for(int i = 0 ; i < L.size() ; i++)
  {
    int v = L[i];


    if (distance[v] < 0 && upperDist > distance[v]) {
      upperHull = v;
      upperDist = distance[v];


    }  else {

      if (distance[v] > 0 && lowerDist < distance[v]) {
        lowerDist = distance[v];
        lowerHull = v;


      } 
    }

    if (distance[v] < 0) {
      label[v] = 1;


    } 



  }

  R[2] = R[1];
  int total = 2;  
  if (upperHull != -1) {
    R[3] = upperHull;
    mark[upperHull] = 1;
    total++;


  }  else {
    R[3] = -1;


  }

  if (lowerHull != -1) {
    R[1] = lowerHull;
    mark[lowerHull] = 1;
    total++;


  }  else {
    R[1] = -1;


  }
  point = innerPoint(R,xCoord,yCoord);
  getSign(R,sign,point,xCoord,yCoord,label,L,mark,2);

  int added = 2;  int currHullSize = 4;  int maxLabel = 2;  
  while (added > 0 && currHullSize <= n ){
    updateLabel(label,R,xCoord,yCoord,mark,currHullSize);

    maxLabel = maxLabel * 2;
    getSign(R,sign,point,xCoord,yCoord,label,L,mark,maxLabel);

    updateDistance(label,distance,xCoord,yCoord,L,R,mark,sign,maxLabel);

    added = updateHull(distance,xCoord,yCoord,R,label,mark,sign,point,L,maxLabel);
    currHullSize = currHullSize * 2;
    total = total + added;



  }
  std::cout<<total<<std::endl;
  return R;


}

int main() {
    int n;
    std::cin >> n;
    std::vector<double> xCoord(n);
    std::vector<double> yCoord(n);

    std::cout << n << std::endl;

    for (int i = 0; i < n; i++) {
        std::cin >> xCoord[i] >> yCoord[i];
        //std::cout << xCoord[i] << " " << yCoord[i] << std::endl;
    }

    std::vector<int> result;

    hipEvent_t start, stop;
    hipEventCreate(&start);
    hipEventCreate(&stop);

    hipEventRecord(start, 0);

    result = convexHull(n, xCoord, yCoord);

    hipEventRecord(stop, 0);
    hipEventSynchronize(stop);

    float elapsedTime;
    hipEventElapsedTime(&elapsedTime, start, stop);
    std::cout << "Time taken for convexHull: " << elapsedTime << " ms" << std::endl;

    hipEventDestroy(start);
    hipEventDestroy(stop);

    //for (int i = 0; i < result.size(); i++) {
    //    std::cout << result[i] << " ";
    //}

    std::cout << std::endl;

    return 0;
}