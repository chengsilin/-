#include <cstdio>
#include <cuda_runtime_api.h>

#include "../morph-linear/morphlib.h"

__global__ void dilation(char *src, char*dst, int width, int height) {
	int x = blockIdx.x * blockDim.x + threadIdx.x;
	int y = blockIdx.y * blockDim.y + threadIdx.y;
	if (x >= width || y >= height)
		return;

	int set = 0;
	for (int i = 0; i < 9; ++i) {
		int x2 = x + (i / 3) - 1;
		int y2 = y + (i % 3) - 1;
		if (x2 >= 0 && y2 >= 0 && x2 < width && y2 < height) {
			int idx2 = y2 * width + x2;
			if (src[idx2]) {
				set = 1;
			}
		}
	}
	
	int idx = y * width + x;
	dst[idx] = set;//set;
}

int ceil_div(int a, int b) {
	return a % b == 0 ? a / b : a / b + 1;
}

void apply_morph(morph::BinaryImage& src, morph::BinaryImage& dst) {
	char *devSrc, *devDst;
	size_t size = src.width * src.height;
	cudaMalloc((void**) &devSrc, size);
	cudaMalloc((void**) &devDst, size);

	dim3 threadsPerBlock = dim3(32, 32);
	//dim3 blocksPerGrid = dim3(1);
	dim3 blocksPerGrid = dim3(ceil_div(src.width, 32), ceil_div(src.height, 32));

	cudaMemcpy(devSrc, src.bytes, size, cudaMemcpyHostToDevice);

	dilation<<<blocksPerGrid, threadsPerBlock>>>(devSrc, devDst, src.width, src.height);

	cudaMemcpy(dst.bytes, devDst, size, cudaMemcpyDeviceToHost);

	cudaFree(devSrc);
	cudaFree(devDst);
}

int main(int argc, char** argv) {
	char* temp;
	cudaMalloc((void**) &temp, 16);
	cudaFree(temp);

	morph::exec_test(argc, argv, apply_morph);
}