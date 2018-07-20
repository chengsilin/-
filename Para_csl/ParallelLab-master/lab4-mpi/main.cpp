#include <mpi.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "../morph-linear/morphlib.h"

const int TagRecvBuf = 123;
const int TagResult = 124;

int world_size, world_rank;
char *srcbuf, *rawdst;

static union {
	struct {
		int width, grid_height;
	};
	int send[2];
};

inline int upper_div(int a, int b) {
	return a % b == 0 ? a / b : (a / b) + 1;
}

void exec_worker();

void exec_root(morph::BinaryImage& src, morph::BinaryImage& dst) {
	width = src.width;
	grid_height = upper_div(src.height, world_size);
	int padded_height = grid_height * world_size;

	// row 0: padded 0's
	// row 1...height: image content
	// row height+1...padded_height: padded 0's
	char* buf = new char[(2 + padded_height) * src.width];
	memcpy(buf + src.width, src.bytes, src.width * src.height);
	memset(buf								 , 0, src.width);
	memset(buf + (1 + src.height) * src.width, 0, (1 + padded_height - src.height) * src.width);

	srcbuf = new char[width * (2 + grid_height)];
	memcpy(srcbuf, buf, width * (2 + grid_height));

	MPI_Request requests[16];
	for (int i = 1; i < world_size; ++i) {
		int offset = src.width * grid_height * i;
		MPI_Isend(buf + offset, src.width * (2 + grid_height), MPI_BYTE, i, TagRecvBuf, MPI_COMM_WORLD, requests + (i - 1));
	}

	rawdst = dst.bytes;
	exec_worker();

	MPI_Waitall(world_size - 1, requests, MPI_STATUSES_IGNORE);

	// Gather result
	for (int i = 1; i < world_size; ++i) {
		MPI_Irecv(dst.bytes + i * width * grid_height, width * grid_height, MPI_BYTE, i, TagResult, MPI_COMM_WORLD, requests + (i - 1));
	}
	MPI_Waitall(world_size - 1, requests, MPI_STATUSES_IGNORE);
}

inline int sample(int x, int y) {
	if (x < 0 || x >= width)
		return 0;
	return srcbuf[(y + 1) * width + x];
}

void exec_worker() {
	// Broadcast necessary metainfo(image width, grid height)
	MPI_Bcast(send, 2, MPI_INT, 0, MPI_COMM_WORLD);
	// Recv 
	if (world_rank != 0) {
		srcbuf = new char[width * (2 + grid_height)];
		MPI_Recv(srcbuf, width * (2 + grid_height), MPI_BYTE, 0, TagRecvBuf, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
	// Perform convolution
	char *result = new char[width * grid_height];
	for (int y = 0; y < grid_height; ++y) {
		for (int x = 0; x < width; ++x) {
			bool hit = false;
			for (int i = 0; i < 9; ++i) {
				int dx = (i % 3) - 1;
				int dy = (i / 3) - 1;
				if (sample(x + dx, y + dy)) {
					hit = true;
					break;
				}
			}
			result[y * width + x] = hit;
		}
	}
	
	// Send back to root process
	if (world_rank == 0) {
		memcpy(rawdst, result, width * grid_height);
	} else {
		MPI_Send(result, width * grid_height, MPI_BYTE, 0, TagResult, MPI_COMM_WORLD);
	}
}

int main(int argc, char** argv) {
	MPI_Init(nullptr, nullptr);

	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	if (world_rank == 0) {
		morph::exec_test(argc, argv, exec_root);
	} else {
		exec_worker();
	}

	MPI_Finalize();
}