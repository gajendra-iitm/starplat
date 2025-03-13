#include "grahamScanConvexHull.h"


auto xComparator(
  Point&  a,Point&  b)
{





  if (a.x != b.x) {
    return a.x < b.x;


  }  else {
    return a.y < b.y;


  }


}
auto crossProduct(
  Point&  p1,Point&  p2,Point&  p3)
{




  return (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);


}
void findTangents(
  std::vector<Point>&  points,int  gid1,int  gid2,int  group_size,
  std::vector<int>&  stackArr,std::vector<int>&  stackSize,std::vector<int>&  tangents)
{




  int start1 = gid1 * group_size;  
  int start2 = gid2 * group_size;  
  int hull1_size = stackSize[gid1];  
  int hull2_size = stackSize[gid2];  
  int a = 0;  
  int temp1 = 1;  

  while (temp1 < hull1_size) {
    int curr = stackArr[start1 + temp1];  
    int rightmost = stackArr[start1 + a];  
    Point curPoint = points[curr];
    Point rightmostPoint = points[rightmost];

    if (curPoint.x > rightmostPoint.x || (curPoint.x == rightmostPoint.x && curPoint.y > rightmostPoint.y)) {
      a = temp1;


    } 
    temp1++;



  }
  int b = 0;  
  temp1 = 1;

  while (temp1 < hull2_size) {
    int curr = stackArr[start2 + temp1];  
    int leftmost = stackArr[start2 + b];  
    Point curPoint = points[curr];
    Point leftmostPoint = points[leftmost];

    if (curPoint.x < leftmostPoint.x || (curPoint.x == leftmostPoint.x && curPoint.y < leftmostPoint.y)) {
      b = temp1;


    } 
    temp1++;



  }
  int upper_a = a;  
  int upper_b = b;  
  bool done = false;  

  while (done == 0) {
    done = true;
    int next_a = (upper_a + 1) % hull1_size;  
    int next_b = (upper_b - 1 + hull2_size) % hull2_size;  
    int curr_point_a = stackArr[start1 + upper_a];  
    int next_point_a = stackArr[start1 + next_a];  
    int curr_point_b = stackArr[start2 + upper_b];  
    int next_point_b = stackArr[start2 + next_b];  

    if (crossProduct(points[curr_point_b],points[curr_point_a],points[next_point_a]) < 0) {
      upper_a = next_a;
      curr_point_a = stackArr[start1 + upper_a];
      done = false;


    }  else {

      if (crossProduct(points[curr_point_b],points[curr_point_a],points[next_point_a]) == 0) {
        Point curPointA = points[curr_point_a];
        Point curPointB = points[curr_point_b];
        Point nextPointA = points[next_point_a];
        double oldDistance = (curPointA.x - curPointB.x) * (curPointA.x - curPointB.x) + (curPointA.y - curPointB.y) * (curPointA.y - curPointB.y);  
        double newDistance = (nextPointA.x - curPointB.x) * (nextPointA.x - curPointB.x) + (nextPointA.y - curPointB.y) * (nextPointA.y - curPointB.y);  

        if (newDistance > oldDistance) {
          upper_a = next_a;
          curr_point_a = stackArr[start1 + upper_a];
          done = false;


        } 


      } 
    }

    if (crossProduct(points[curr_point_a],points[curr_point_b],points[next_point_b]) > 0) {
      upper_b = next_b;
      done = false;


    }  else {

      if (crossProduct(points[curr_point_a],points[curr_point_b],points[next_point_b]) == 0) {
        Point curPointB = points[curr_point_b];
        Point curPointA = points[curr_point_a];
        Point nextPointB = points[next_point_b];
        double oldDistance = (curPointB.x - curPointA.x) * (curPointB.x - curPointA.x) + (curPointB.y - curPointA.y) * (curPointB.y - curPointA.y);  
        double newDistance = (nextPointB.x - curPointA.x) * (nextPointB.x - curPointA.x) + (nextPointB.y - curPointA.y) * (nextPointB.y - curPointA.y);  

        if (newDistance > oldDistance) {
          upper_b = next_b;
          done = false;


        } 


      } 
    }



  }
  int lower_a = a;  
  int lower_b = b;  
  done = false;

  while (done == 0) {
    done = true;
    int next_a = (lower_a - 1 + hull1_size) % hull1_size;  
    int next_b = (lower_b + 1) % hull2_size;  
    int curr_point_a = stackArr[start1 + lower_a];  
    int next_point_a = stackArr[start1 + next_a];  
    int curr_point_b = stackArr[start2 + lower_b];  
    int next_point_b = stackArr[start2 + next_b];  

    if (crossProduct(points[curr_point_b],points[curr_point_a],points[next_point_a]) > 0) {
      lower_a = next_a;
      curr_point_a = stackArr[start1 + lower_a];
      done = false;


    }  else {

      if (crossProduct(points[curr_point_b],points[curr_point_a],points[next_point_a]) == 0) {
        Point curPointA = points[curr_point_a];
        Point curPointB = points[curr_point_b];
        Point nextPointA = points[next_point_a];
        double oldDistance = (curPointA.x - curPointB.x) * (curPointA.x - curPointB.x) + (curPointA.y - curPointB.y) * (curPointA.y - curPointB.y);  
        double newDistance = (nextPointA.x - curPointB.x) * (nextPointA.x - curPointB.x) + (nextPointA.y - curPointB.y) * (nextPointA.y - curPointB.y);  

        if (newDistance > oldDistance) {
          lower_a = next_a;
          curr_point_a = stackArr[start1 + lower_a];
          done = false;


        } 


      } 
    }

    if (crossProduct(points[curr_point_a],points[curr_point_b],points[next_point_b]) < 0) {
      lower_b = next_b;
      done = false;


    }  else {

      if (crossProduct(points[curr_point_a],points[curr_point_b],points[next_point_b]) == 0) {
        Point curPointA = points[curr_point_a];
        Point curPointB = points[curr_point_b];
        Point nextPointB = points[next_point_b];
        double oldDistance = (curPointA.x - curPointB.x) * (curPointA.x - curPointB.x) + (curPointA.y - curPointB.y) * (curPointA.y - curPointB.y);  
        double newDistance = (nextPointB.x - curPointA.x) * (nextPointB.x - curPointA.x) + (nextPointB.y - curPointA.y) * (nextPointB.y - curPointA.y);  

        if (newDistance > oldDistance) {
          lower_b = next_b;
          done = false;


        } 


      } 
    }



  }
  tangents[0] = upper_a;
  tangents[1] = upper_b;
  tangents[2] = lower_a;
  tangents[3] = lower_b;


}
void mergeTwoHulls(
  std::vector<Point>&  points,int  gid1,int  gid2,int  group_size,
  std::vector<int>&  stackArr,std::vector<int>&  stackSize)
{




  std::vector<int> tangents(4);
  findTangents(points,gid1,gid2,group_size,stackArr,stackSize,tangents);

  int start1 = gid1 * group_size;  
  int start2 = gid2 * group_size;  
  int hull1_size = stackSize[gid1];  
  int hull2_size = stackSize[gid2];  
  int upper_a = tangents[0];  
  int upper_b = tangents[1];  
  int lower_a = tangents[2];  
  int lower_b = tangents[3];  
  std::vector<int> mergedHull;
  int curIdx = upper_a;  

  while (curIdx != lower_a) {
    mergedHull.push_back(stackArr[start1 + curIdx]);

    curIdx = (curIdx + 1) % hull1_size;



  }
  mergedHull.push_back(stackArr[start1 + lower_a]);

  curIdx = lower_b;

  while (curIdx != upper_b) {
    mergedHull.push_back(stackArr[start2 + curIdx]);

    curIdx = (curIdx + 1) % hull2_size;



  }
  mergedHull.push_back(stackArr[start2 + upper_b]);

  curIdx = 0;

  while (curIdx < mergedHull.size()) {
    stackArr[start1 + curIdx] = mergedHull[curIdx];
    curIdx = curIdx + 1;



  }
  stackSize[gid1] = mergedHull.size();
  stackSize[gid2] = 0;


}
auto grahamScanConvexHull(
  int  n,std::vector<Point>&  points,int  k)
{




  int group_size = (n + k - 1) / k;  
  thrust::device_vector<Point> d_points = points;
  thrust::sort(d_points.begin(), d_points.end(), [] __host__ __device__(Point& a, Point& b){

    if (a.x != b.x) {
      return a.x < b.x;


    }  else {
      return a.y < b.y;


    }
  });
  thrust::copy(d_points.begin(), d_points.end(), points.begin());

  std::vector<int> L(k);
  std::vector<int> stackArr(n, 0);
  std::vector<int> stackSize(k, 0);
  int* d_L0;
  hipMalloc(&d_L0, sizeof(int) * L.size());
  hipMemcpy(d_L0, L.data(), L.size() * sizeof(int), hipMemcpyHostToDevice);

  int V0 = L.size();
  const unsigned threadsPerBlock0 = 32;
  const unsigned numBlocks0 = (V0 + threadsPerBlock0 - 1) / threadsPerBlock0;

  grahamScanConvexHull_kernel0<<<numBlocks0, threadsPerBlock0>>>(V0, d_L0);
  hipDeviceSynchronize();
  hipMemcpy(L.data(), d_L0, L.size() * sizeof(int), hipMemcpyDeviceToHost);
  hipFree(d_L0);
  int* d_L1;
  hipMalloc(&d_L1, sizeof(int) * L.size());
  hipMemcpy(d_L1, L.data(), L.size() * sizeof(int), hipMemcpyHostToDevice);
  Point* d_points1;
  hipMalloc(&d_points1, sizeof(Point) * points.size());
  hipMemcpy(d_points1, points.data(), points.size() * sizeof(Point), hipMemcpyHostToDevice);
  int* d_stackSize1;
  hipMalloc(&d_stackSize1, sizeof(int) * stackSize.size());
  hipMemcpy(d_stackSize1, stackSize.data(), stackSize.size() * sizeof(int), hipMemcpyHostToDevice);
  int* d_stackArr1;
  hipMalloc(&d_stackArr1, sizeof(int) * stackArr.size());
  hipMemcpy(d_stackArr1, stackArr.data(), stackArr.size() * sizeof(int), hipMemcpyHostToDevice);

  int V1 = L.size();
  const unsigned threadsPerBlock1 = 32;
  const unsigned numBlocks1 = (V1 + threadsPerBlock1 - 1) / threadsPerBlock1;

  grahamScanConvexHull_kernel1<<<numBlocks1, threadsPerBlock1>>>(V1, d_L1, d_points1, d_stackSize1, d_stackArr1, n, group_size);
  hipDeviceSynchronize();
  hipMemcpy(L.data(), d_L1, L.size() * sizeof(int), hipMemcpyDeviceToHost);
  hipFree(d_L1);
  hipMemcpy(points.data(), d_points1, points.size() * sizeof(Point), hipMemcpyDeviceToHost);
  hipFree(d_points1);
  hipMemcpy(stackSize.data(), d_stackSize1, stackSize.size() * sizeof(int), hipMemcpyDeviceToHost);
  hipFree(d_stackSize1);
  hipMemcpy(stackArr.data(), d_stackArr1, stackArr.size() * sizeof(int), hipMemcpyDeviceToHost);
  hipFree(d_stackArr1);
















  int curIdx = 1;  

  while (curIdx < k) {

    if (stackSize[curIdx] != 0) {
      mergeTwoHulls(points,0,curIdx,group_size,stackArr,stackSize);



    } 
    curIdx = curIdx + 1;



  }
  std::vector<Point> finalHull;
  int elemNum = 0;  

  while (elemNum < stackSize[0]) {
    finalHull.push_back(points[stackArr[elemNum]]);

    elemNum = elemNum + 1;



  }
  return finalHull;


}

int main(int argc, char **argv)
{
  int k = atoi(argv[1]);
  char *input_file = argv[2];
  std::ifstream file(input_file);
  int n;
  file >> n;
  std::vector<Point> points(n);
  std::cout << "Number of points: " << n << std::endl;
  for (int i = 0; i < n; i++)
  {
    double x, y;
    file >> x >> y;
    points[i] = Point(x, y);
  }
  hipEvent_t start, stop;
  hipEventCreate(&start);
  hipEventCreate(&stop);
  hipEventRecord(start, 0);
  std::vector<Point> finalConvexHull = grahamScanConvexHull(n, points, k);
  hipEventRecord(stop, 0);
  hipEventSynchronize(stop);
  float elapsedTime;
  hipEventElapsedTime(&elapsedTime, start, stop);
  std::cout << "Time taken for grahamScanConvexHull: " << elapsedTime << " ms" << std::endl;
  hipEventDestroy(start);
  hipEventDestroy(stop);

  printf("Final convex hull size: %d\n", finalConvexHull.size());
  // sort the points first based on x and then based on y
  std::sort(finalConvexHull.begin(), finalConvexHull.end(), xComparator);
  for (int i = 0; i < finalConvexHull.size(); i++)
  {
    printf("[%lf %lf]\n", finalConvexHull[i].x, finalConvexHull[i].y);
  }
  std::cout << std::endl;
  return 0;
}
