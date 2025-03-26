#include "grahamScanConvexHull.h"

auto xComparator(
    Point &a, Point &b)
{
  if (a.x != b.x)
  {
    return a.x < b.x;
  }
  else
  {
    return a.y < b.y;
  }
}
auto crossProduct(
    Point &p1, Point &p2, Point &p3)
{

  return (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);
}
auto findExtremePoints(
    std::vector<Point> &points, int start, int hull_size, std::vector<int> &stackArr,
    bool findRightmost)
{

  int extremeIdx = 0;
  int tempIdx = 1;

  while (tempIdx < hull_size)
  {
    int curr = stackArr[start + tempIdx];
    int extreme = stackArr[start + extremeIdx];
    Point curPoint = points[curr];
    Point extremePoint = points[extreme];

    if (findRightmost == 1)
    {

      if (curPoint.x > extremePoint.x || (curPoint.x == extremePoint.x && curPoint.y > extremePoint.y))
      {
        extremeIdx = tempIdx;
      }
    }
    else
    {

      if (curPoint.x < extremePoint.x || (curPoint.x == extremePoint.x && curPoint.y < extremePoint.y))
      {
        extremeIdx = tempIdx;
      }
    }
    tempIdx++;
  }
  return extremeIdx;
}
auto updateTangent(
    bool done, int movingHullStart, int movingPoint, int movingHullSize,
    int pivotHullStart, int pivotPoint, bool isCCW, std::vector<Point> &points,
    std::vector<int> &stackArr)
{

  int nextPoint;

  if (isCCW == 1)
  {
    nextPoint = (movingPoint + 1) % movingHullSize;
  }
  else
  {
    nextPoint = (movingPoint - 1 + movingHullSize) % movingHullSize;
  }
  int curMovingPointInd = stackArr[movingHullStart + movingPoint];
  int curPivotPointInd = stackArr[pivotHullStart + pivotPoint];
  int nextMovingPointInd = stackArr[movingHullStart + nextPoint];

  if ((crossProduct(points[curPivotPointInd], points[curMovingPointInd], points[nextMovingPointInd]) < 0 && isCCW == 1) || (crossProduct(points[curPivotPointInd], points[curMovingPointInd], points[nextMovingPointInd]) > 0 && isCCW == 0))
  {
    movingPoint = nextPoint;
    done = false;
  }
  else
  {

    if (crossProduct(points[curPivotPointInd], points[curMovingPointInd], points[nextMovingPointInd]) == 0)
    {
      Point curMovingPoint = points[curMovingPointInd];
      Point curPivotPoint = points[curPivotPointInd];
      Point nextMovingPoint = points[nextMovingPointInd];
      double oldDistance = (curMovingPoint.x - curPivotPoint.x) * (curMovingPoint.x - curPivotPoint.x) + (curMovingPoint.y - curPivotPoint.y) * (curMovingPoint.y - curPivotPoint.y);
      double newDistance = (nextMovingPoint.x - curPivotPoint.x) * (nextMovingPoint.x - curPivotPoint.x) + (nextMovingPoint.y - curPivotPoint.y) * (nextMovingPoint.y - curPivotPoint.y);

      if (newDistance > oldDistance)
      {
        movingPoint = nextPoint;
        done = false;
      }
    }
  }
  std::vector<int> result(2);
  result[0] = movingPoint;
  result[1] = done;
  return result;
}
void findTangents(
    std::vector<Point> &points, int gid1, int gid2, int group_size,
    std::vector<int> &stackArr, std::vector<int> &stackSize, std::vector<int> &tangents)
{

  int start1 = gid1 * group_size;
  int start2 = gid2 * group_size;
  int hull1_size = stackSize[gid1];
  int hull2_size = stackSize[gid2];
  int upper_a = findExtremePoints(points, start1, hull1_size, stackArr, true);
  int upper_b = findExtremePoints(points, start2, hull2_size, stackArr, false);
  int lower_a = upper_a;
  int lower_b = upper_b;
  bool done = false;
  std::vector<int> result(2);

  while (done == 0)
  {
    done = true;
    result = updateTangent(done, start1, upper_a, hull1_size, start2, upper_b, true, points, stackArr);
    upper_a = result[0];
    done = done && (result[1] == 1);
    result = updateTangent(done, start2, upper_b, hull2_size, start1, upper_a, false, points, stackArr);
    upper_b = result[0];
    done = done && (result[1] == 1);
  }
  done = false;

  while (done == 0)
  {
    done = true;
    result = updateTangent(done, start1, lower_a, hull1_size, start2, lower_b, false, points, stackArr);
    lower_a = result[0];
    done = done && (result[1] == 1);
    result = updateTangent(done, start2, lower_b, hull2_size, start1, lower_a, true, points, stackArr);
    lower_b = result[0];
    done = done && (result[1] == 1);
  }
  tangents[0] = upper_a;
  tangents[1] = upper_b;
  tangents[2] = lower_a;
  tangents[3] = lower_b;
}
void mergeTwoHulls(
    std::vector<Point> &points, int gid1, int gid2, int group_size,
    std::vector<int> &stackArr, std::vector<int> &stackSize)
{

  std::vector<int> tangents(4);
  findTangents(points, gid1, gid2, group_size, stackArr, stackSize, tangents);

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

  while (curIdx != lower_a)
  {
    mergedHull.push_back(stackArr[start1 + curIdx]);

    curIdx = (curIdx + 1) % hull1_size;
  }
  mergedHull.push_back(stackArr[start1 + lower_a]);

  curIdx = lower_b;

  while (curIdx != upper_b)
  {
    mergedHull.push_back(stackArr[start2 + curIdx]);

    curIdx = (curIdx + 1) % hull2_size;
  }
  mergedHull.push_back(stackArr[start2 + upper_b]);

  curIdx = 0;

  while (curIdx < mergedHull.size())
  {
    stackArr[start1 + curIdx] = mergedHull[curIdx];
    curIdx = curIdx + 1;
  }
  stackSize[gid1] = mergedHull.size();
  stackSize[gid2] = 0;
}
auto grahamScanConvexHull(
    int n, std::vector<Point> &points, int k)
{

  int group_size = (n + k - 1) / k;
  thrust::device_vector<Point> d_points = points;
  thrust::sort(d_points.begin(), d_points.end(), [] __host__ __device__(Point & a, Point & b)
               {

    if (a.x != b.x) {
      return a.x < b.x;


    }  else {
      return a.y < b.y;


    } });
  thrust::copy(d_points.begin(), d_points.end(), points.begin());

  std::vector<int> L(k);
  std::vector<int> stackArr(n, 0);
  std::vector<int> stackSize(k, 0);
  int *d_L0;
  hipMalloc(&d_L0, sizeof(int) * L.size());
  hipMemcpy(d_L0, L.data(), L.size() * sizeof(int), hipMemcpyHostToDevice);

  int V0 = L.size();
  const unsigned threadsPerBlock0 = 32;
  const unsigned numBlocks0 = (V0 + threadsPerBlock0 - 1) / threadsPerBlock0;

  grahamScanConvexHull_kernel0<<<numBlocks0, threadsPerBlock0>>>(V0, d_L0);
  hipDeviceSynchronize();
  hipMemcpy(L.data(), d_L0, L.size() * sizeof(int), hipMemcpyDeviceToHost);
  hipFree(d_L0);
  int *d_L1;
  hipMalloc(&d_L1, sizeof(int) * L.size());
  hipMemcpy(d_L1, L.data(), L.size() * sizeof(int), hipMemcpyHostToDevice);
  int *d_stackSize1;
  hipMalloc(&d_stackSize1, sizeof(int) * stackSize.size());
  hipMemcpy(d_stackSize1, stackSize.data(), stackSize.size() * sizeof(int), hipMemcpyHostToDevice);
  int *d_stackArr1;
  hipMalloc(&d_stackArr1, sizeof(int) * stackArr.size());
  hipMemcpy(d_stackArr1, stackArr.data(), stackArr.size() * sizeof(int), hipMemcpyHostToDevice);
  Point *d_points1;
  hipMalloc(&d_points1, sizeof(Point) * points.size());
  hipMemcpy(d_points1, points.data(), points.size() * sizeof(Point), hipMemcpyHostToDevice);

  int V1 = L.size();
  const unsigned threadsPerBlock1 = 32;
  const unsigned numBlocks1 = (V1 + threadsPerBlock1 - 1) / threadsPerBlock1;

  grahamScanConvexHull_kernel1<<<numBlocks1, threadsPerBlock1>>>(V1, d_L1, d_stackSize1, d_stackArr1, d_points1, n, group_size);
  hipDeviceSynchronize();
  hipMemcpy(L.data(), d_L1, L.size() * sizeof(int), hipMemcpyDeviceToHost);
  hipFree(d_L1);
  hipMemcpy(stackSize.data(), d_stackSize1, stackSize.size() * sizeof(int), hipMemcpyDeviceToHost);
  hipFree(d_stackSize1);
  hipMemcpy(stackArr.data(), d_stackArr1, stackArr.size() * sizeof(int), hipMemcpyDeviceToHost);
  hipFree(d_stackArr1);
  hipMemcpy(points.data(), d_points1, points.size() * sizeof(Point), hipMemcpyDeviceToHost);
  hipFree(d_points1);

  int curIdx = 1;

  while (curIdx < k)
  {

    if (stackSize[curIdx] != 0)
    {
      mergeTwoHulls(points, 0, curIdx, group_size, stackArr, stackSize);
    }
    curIdx = curIdx + 1;
  }
  std::vector<Point> finalHull;
  int elemNum = 0;

  while (elemNum < stackSize[0])
  {
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
