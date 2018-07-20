#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


__global__ void VecAdd(int* A, int* B, int* C, int N) {
	int i = blockDim.x * blockIdx.x + threadIdx.x;
	if (i < N) {
		A[i] = B[i] + C[i];
	}
}

constexpr int N = 1024;

int A[N], B[N], C[N];

int main() {
	for (int i = 0; i < N; ++i) {
		B[i] = i + 1;
		C[i] = 2 * i - 3;
	}

	int *dstA, *dstB, *dstC;
	cudaMalloc((void**) &dstA, N * sizeof(int));
	cudaMalloc((void**) &dstB, N * sizeof(int));
	cudaMalloc((void**) &dstC, N * sizeof(int));
	
	int threadsPerBlock = 256;
	int blocksPerGrid = (N + threadsPerBlock - 1) / threadsPerBlock;

	cudaMemcpy(dstB, B, sizeof(int) * N, cudaMemcpyHostToDevice);
	cudaMemcpy(dstC, C, sizeof(int) * N, cudaMemcpyHostToDevice);

	// Invoke kernel
	VecAdd<<<blocksPerGrid, threadsPerBlock>>>(dstA, dstB, dstC, N);
	
	// Copy mem from device to host
	cudaMemcpy(A, dstA, N * sizeof(int), cudaMemcpyDeviceToHost);

	// Free device memory
	cudaFree(dstA);
	cudaFree(dstB);
	cudaFree(dstC);

	// Print result
	printf("Results: ");
	for (int i = 0; i < N; ++i) {
		printf("%d ", A[i]);
	}
	printf("\n");
}