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

//actual function for each thread
__global__ void HELP(const int *a, const int *b, int *c, int N) {
 
  int blX=blockIdx.x * blockDim.x;
  int blY=blockIdx.y * blockDim.y;
  int R =  blX + threadIdx.x;
  int C =  blY+ threadIdx.y;

  //int temp;
  //
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



void gpuThread(int N, int *matA, int *matB, int *output)
{   

  int big_square=N*N;
 int *MatrixA_d, *MatrixB_d, *Outmat_d;

  // allocate device memory by cuda malloc
    cudaMalloc((void**)&MatrixA_d, sizeof(int)*big_square);
    cudaMalloc((void**)&MatrixB_d, sizeof(int)*big_square);
    cudaMalloc((void**)&Outmat_d, sizeof(int)*big_square/4);

    // copy data from host to to device
    cudaMemcpy(MatrixA_d, matA, sizeof(int)*big_square, cudaMemcpyHostToDevice);
    cudaMemcpy(MatrixB_d, matB, sizeof(int)*big_square, cudaMemcpyHostToDevice);



  // Threads per block grid in 1d
  int Thread_COUNT = 4;

  int row_output=N>>1;
  int Block_COUNT = row_output / Thread_COUNT;


  //approprioate threads are used(16 per block)
  //create dimension
  dim3 threads(Thread_COUNT, Thread_COUNT);
  dim3 blocks(Block_COUNT, Block_COUNT);

  // call the HELP
  auto begin = TIME_NOW;
  HELP<<<blocks, threads>>>(MatrixA_d, MatrixB_d, Outmat_d, N);
  auto end = TIME_NOW;
  cout<<"---------------------------  executed successfully on GPU   ---------------------------------------- "<<N<<endl;
  cout << "GPU Multi-threaded execution time: " << (double)TIME_DIFF(std::chrono::microseconds, begin, end) / 1000.0 << " ms\n";

  //giving back to the CPU
  cudaMemcpy(output, Outmat_d, sizeof(int)*big_square/4, cudaMemcpyDeviceToHost);

  //we free thee memory
  cudaFree(MatrixA_d);
  cudaFree(MatrixB_d);
  cudaFree(Outmat_d);

}
