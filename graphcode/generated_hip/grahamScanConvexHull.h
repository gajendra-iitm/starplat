#ifndef GENHIP_GRAHAMSCANCONVEXHULL_H
#define GENHIP_GRAHAMSCANCONVEXHULL_H

#include <iostream>
#include <climits>
#include <hip/hip_runtime.h>
#include <hip/hip_cooperative_groups.h>
#include "../graph.hpp"
#include <thrust/sort.h>
#include <thrust/device_vector.h>
#include <thrust/copy.h>

auto xComparator(
    Point &a, Point &b);

auto crossProduct(
    Point &p1, Point &p2, Point &p3);

auto findExtremePoints(
    std::vector<Point> &points, int start, int hull_size, std::vector<int> &stackArr,
    bool findRightmost);

// DEVICE ASSTMENT in .h
// DEVICE ASSTMENT in .h
// DEVICE ASSTMENT in .h
// DEVICE ASSTMENT in .h
auto updateTangent(
    bool done, int movingHullStart, int movingPoint, int movingHullSize,
    int pivotHullStart, int pivotPoint, bool isCCW, std::vector<Point> &points,
    std::vector<int> &stackArr);

// DEVICE ASSTMENT in .h
// DEVICE ASSTMENT in .h
// DEVICE ASSTMENT in .h
// DEVICE ASSTMENT in .h
// DEVICE ASSTMENT in .h
// DEVICE ASSTMENT in .h
void findTangents(
    std::vector<Point> &points, int gid1, int gid2, int group_size,
    std::vector<int> &stackArr, std::vector<int> &stackSize, std::vector<int> &tangents);

// DEVICE ASSTMENT in .h
// DEVICE ASSTMENT in .h
// DEVICE ASSTMENT in .h
// DEVICE ASSTMENT in .h
// DEVICE ASSTMENT in .h
// DEVICE ASSTMENT in .h
// DEVICE ASSTMENT in .h
// DEVICE ASSTMENT in .h
// DEVICE ASSTMENT in .h
void mergeTwoHulls(
    std::vector<Point> &points, int gid1, int gid2, int group_size,
    std::vector<int> &stackArr, std::vector<int> &stackSize);

// DEVICE ASSTMENT in .h
// DEVICE ASSTMENT in .h
// DEVICE ASSTMENT in .h
// DEVICE ASSTMENT in .h
// DEVICE ASSTMENT in .h
// DEVICE ASSTMENT in .h
// DEVICE ASSTMENT in .h
// DEVICE ASSTMENT in .h
// DEVICE ASSTMENT in .h
auto grahamScanConvexHull(
    int n, std::vector<Point> &points, int k);

// DEVICE ASSTMENT in .h
__global__ void grahamScanConvexHull_kernel0(int V, int *L)
{ // BEGIN KER FUN via ADDKERNEL
  unsigned v = blockIdx.x * blockDim.x + threadIdx.x;
  if (v >= V)
    return;
  L[v] = v;
} // end KER FUNC

__global__ void grahamScanConvexHull_kernel1(int V, int *L, int *stackSize, int *stackArr, Point *points, int n, int group_size)
{ // BEGIN KER FUN via ADDKERNEL
  unsigned gid = blockIdx.x * blockDim.x + threadIdx.x;
  if (gid >= V)
    return;
  int start = gid * group_size;
  // DEVICE ASSTMENT in .h
  int end;
  // DEVICE ASSTMENT in .h
  end = min((gid + 1) * group_size, n);

  if (start >= n)
  {
    stackSize[gid] = 0;
  }
  else
  {
    int pivotIdx = start;
    // DEVICE ASSTMENT in .h
    Point startPoint = points[start];
    double lowestY = startPoint.y;
    // DEVICE ASSTMENT in .h
    int temp = start + 1;
    // DEVICE ASSTMENT in .h

    while (temp < end)
    {
      Point currentPivot = points[pivotIdx];
      Point tempPoint = points[temp];

      if (tempPoint.y < lowestY || (tempPoint.y == lowestY && tempPoint.x < currentPivot.x))
      {
        pivotIdx = temp;
        lowestY = tempPoint.y;
      }
      atomicAdd(&temp, 1);
    }
    Point pivot = points[pivotIdx];
    temp = start;

    while (temp < end)
    {
      int temp2 = temp + 1;
      // DEVICE ASSTMENT in .h

      while (temp2 < end)
      {
        Point tempPoint = points[temp];
        Point temp2Point = points[temp2];
        double dx1 = tempPoint.x - pivot.x;
        // DEVICE ASSTMENT in .h
        double dy1 = tempPoint.y - pivot.y;
        // DEVICE ASSTMENT in .h
        double dx2 = temp2Point.x - pivot.x;
        // DEVICE ASSTMENT in .h
        double dy2 = temp2Point.y - pivot.y;
        // DEVICE ASSTMENT in .h
        double cross = dx1 * dy2 - dy1 * dx2;
        // DEVICE ASSTMENT in .h

        if (cross == 0)
        {
          double dist1 = dx1 * dx1 + dy1 * dy1;
          // DEVICE ASSTMENT in .h
          double dist2 = dx2 * dx2 + dy2 * dy2;
          // DEVICE ASSTMENT in .h

          if (dist2 < dist1)
          {
            Point temp_swap = points[temp];
            points[temp] = points[temp2];
            points[temp2] = temp_swap;
          }
        }
        else
        {

          if (cross < 0)
          {
            Point temp_swap = points[temp];
            points[temp] = points[temp2];
            points[temp2] = temp_swap;
          }
        }
        atomicAdd(&temp2, 1);
      }
      atomicAdd(&temp, 1);
    }

    if (end - start < 3)
    {
      int temp = start;
      // DEVICE ASSTMENT in .h

      while (temp < end)
      {
        stackArr[temp] = temp;
        atomicAdd(&temp, 1);
      }
      stackSize[gid] = end - start;
    }
    else
    {
      int stackIndex = gid * group_size;
      // DEVICE ASSTMENT in .h
      stackArr[stackIndex] = start;
      stackArr[stackIndex + 1] = start + 1;
      stackSize[gid] = 2;
      temp = start + 2;

      while (temp < end)
      {
        bool shouldContinuePopping = true;
        // DEVICE ASSTMENT in .h

        while (stackSize[gid] >= 2 && shouldContinuePopping)
        {
          int topIdx = stackIndex + stackSize[gid] - 1;
          // DEVICE ASSTMENT in .h
          int top = stackArr[topIdx];
          // DEVICE ASSTMENT in .h
          int nextTopIdx = stackIndex + stackSize[gid] - 2;
          // DEVICE ASSTMENT in .h
          int nextTop = stackArr[nextTopIdx];
          // DEVICE ASSTMENT in .h
          Point topPoint = points[top];
          Point nextTopPoint = points[nextTop];
          Point tempPoint = points[temp];
          double cross = (nextTopPoint.x - topPoint.x) * (tempPoint.y - topPoint.y) - (nextTopPoint.y - topPoint.y) * (tempPoint.x - topPoint.x);
          // DEVICE ASSTMENT in .h

          if (cross < 0)
          {
            shouldContinuePopping = false;
          }
          else
          {
            stackSize[gid] = stackSize[gid] - 1;
          }
        }
        stackArr[stackIndex + stackSize[gid]] = temp;
        stackSize[gid] = stackSize[gid] + 1;
        atomicAdd(&temp, 1);
      }
    }
  }
} // end KER FUNC

// DEVICE ASSTMENT in .h
// DEVICE ASSTMENT in .h

#endif
