#include "..\morph-linear\morphlib.h"

#include <thread>
#include <vector>

// Apply erosion operation starting form pixel <offset> for <count> pixels.
void worker(morph::BinaryImage* src, morph::BinaryImage* dst, int offset, int count) {
	for (int i = offset; i < offset + count; ++i) {
		int y = i / src->width;
		int x = i % src->width;
		bool hit = false;
		for (int j = 0; j < 9; ++j) {
			int dy = (j / 3) - 1;
			int dx = (j % 3) - 1;
			if (src->pixel(x + dx, y + dy)) {
				hit = true;
				break;
			}
		}
		//printf("(%d %d) ", x, y);
		dst->set_pixel(x, y, hit);
	}
}

void apply_erosion(morph::BinaryImage& src, morph::BinaryImage& dst) {
	int thread_count = 16;
	int size = src.width * src.height;
	int per_thread = size / thread_count;

	//for (int i = 0; i < thread_count; ++i) {
	//	worker(&src, &dst, per_thread * i, i == thread_count - 1 ? per_thread + (size % thread_count) : per_thread);
	//}

	std::vector<std::thread> threads;
	for (int i = 0; i < thread_count; ++i) {
		threads.emplace_back(worker, &src, &dst, per_thread * i,
			i == thread_count - 1 ? per_thread + (size % thread_count) : per_thread);
	}

	for (auto& th : threads) {
		th.join();
	}
}

int main(int argc, char** argv) {
	morph::exec_test(argc, argv, apply_erosion);
}