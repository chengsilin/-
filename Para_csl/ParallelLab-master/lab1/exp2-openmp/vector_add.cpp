#include <cstdio>
#include <cstdlib>
#include <omp.h>

using namespace std;

constexpr int N = 128;

int A[N], B[N], C[N];

int main() {
	// initialize B and C
	for (int i = 0; i < N; ++i) {
		B[i] = i + 1;
		C[i] = 2 * i - 1;
	}

	// Add stuffs
#pragma omp parallel for
	for (int i = 0; i < N; ++i) {
		int tid = omp_get_thread_num();
		A[i] = B[i] + C[i];
		printf("Task %d from thread %d, result %d\n", i, tid, A[i]);
	}

	printf("Final result: ");
	for (int i = 0; i < N; ++i) {
		printf("%d ", A[i]);
	}
	printf("\n");

	system("pause");
}