//Necessory header files
#include <functional>
#include <iostream>
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <vector>


#define TIME_NOW std::chrono::high_resolution_clock::now()
#define TIME_DIFF(gran, start, end) std::chrono::duration_cast<gran>(end - start).count()


//namespaces
using std::cout;
using std::generate;
using std::vector;

__global__ void HELP(const int *a, const int *b, int *c, int N) {
 

  int R = blockIdx.x * blockDim.x + threadIdx.x;
  int C = blockIdx.y * blockDim.y + threadIdx.y;
  //int temp;
  int ind=R * (N/2) + C;
  c[ind] = 0;
  for (int k = 0; k < N; k++) {
    //add all 4 elements
    
    c[ind] += a[((R<<1)) * N + k] * b[k * N + ((C<<1)+1)];
    c[ind] += a[((R<<1)+1) * N + k] * b[k * N + ((C<<1)+1)];
    c[ind] += a[((R<<1)) * N + k] * b[k * N + ((C<<1))];
    c[ind] += a[((R<<1)+1) * N + k] * b[k * N + ((C<<1))];


  }
}
// NEw commentaiuhdiuhwaiudhuiawd


void gpuThread(int N, int *matA, int *matB, int *output)
{   


 int *MatrixA_d, *MatrixB_d, *Outmat_d;
  
  // allocate device memory 
    cudaMalloc((void**)&MatrixA_d, sizeof(int)*N*N);
    cudaMalloc((void**)&MatrixB_d, sizeof(int)*N*N);
    cudaMalloc((void**)&Outmat_d, sizeof(int)*N*N/4);

    // copy data from host to to device
    cudaMemcpy(MatrixA_d, matA, sizeof(int)*N*N, cudaMemcpyHostToDevice);
    cudaMemcpy(MatrixB_d, matB, sizeof(int)*N*N, cudaMemcpyHostToDevice);



  // Threads per CTA dimension
  int Thread_COUNT = 4;

  // Blocks per grid dimension (assumes Thread_COUNT divides N evenly)
  int Block_COUNT = (N/2) / Thread_COUNT;

  // Use dim3 structs for block  and grid dimensions
  dim3 threads(Thread_COUNT, Thread_COUNT);
  dim3 blocks(Block_COUNT, Block_COUNT);

  // Launch kernel
  auto begin = TIME_NOW;
  HELP<<<blocks, threads>>>(MatrixA_d, MatrixB_d, Outmat_d, N);
  auto end = TIME_NOW;
  cout<<"---------------------------  executed successfully on GPU   ---------------------------------------- "<<N<<endl;
  cout << "GPU Multi-threaded execution time: " << (double)TIME_DIFF(std::chrono::microseconds, begin, end) / 1000.0 << " ms\n";

  // Copy back to the host
  cudaMemcpy(output, Outmat_d, sizeof(int)*N*N/4, cudaMemcpyDeviceToHost);

  // Free memory on device
  cudaFree(MatrixA_d);
  cudaFree(MatrixB_d);
  cudaFree(Outmat_d);

}
