#include <mpi.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>

constexpr int N = 128;

int main() {
	MPI_Init(nullptr, nullptr);

	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	int *A = nullptr, *B = nullptr, *C = nullptr;
	if (world_rank == 0) {
		// Only create buffer in root process
		A = new int[N];
		B = new int[N];
		C = new int[N];
		// initialize B and C
		for (int i = 0; i < N; ++i) {
			B[i] = i + 1;
			C[i] = 2 * i - 1;
		}
	}

	int elems_per_proc = N / world_size;
	if (elems_per_proc * world_size != N) {
		return -1;
	}

	int *B_recv = new int[elems_per_proc], *C_recv = new int[elems_per_proc];
	MPI_Scatter(B, elems_per_proc, MPI_INT, B_recv, elems_per_proc, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatter(C, elems_per_proc, MPI_INT, C_recv, elems_per_proc, MPI_INT, 0, MPI_COMM_WORLD);

	// Add elements distributed to this process
	for (int i = 0; i < elems_per_proc; ++i) {
		B_recv[i] = B_recv[i] + C_recv[i]; // To reduce mem usage, B is used to hold the result.
	}

	printf("Worker %d processed %d elements' additions.\n", world_rank, elems_per_proc);

	// Gather calculation result to root process
	MPI_Gather(B_recv, elems_per_proc, MPI_INT, A, elems_per_proc, MPI_INT, 0, MPI_COMM_WORLD);

	if (world_rank == 0) {
		printf("Results: ");
		for (int i = 0; i < N; ++i) {
			printf("%d ", A[i]);
		}
		printf("\n");
	}

	MPI_Finalize();
}