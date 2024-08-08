#include <cuda.h>

// Matrix multiplication kernel for 2 1-D row major arrays
// As parellel as possible and dimensions are not equal
template <typename T>
__global__ void matrixMulRowMajor(T *A, T *B, T *C, int N, int M, int K)
{
  int row = blockIdx.y * blockDim.y + threadIdx.y;
  int col = blockIdx.x * blockDim.x + threadIdx.x;
  if (row < N && col < K)
  {
    T sum = 0;
    for (int i = 0; i < M; i++)
    {
      sum += A[row * M + i] * B[i * K + col];
    }
    C[row * K + col] = sum;
  }
}
