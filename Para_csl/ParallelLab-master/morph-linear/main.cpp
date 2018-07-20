#include <iostream>
#include "morphlib.h"

using namespace std;
using namespace morph;

void apply_erosion(BinaryImage& img, BinaryImage& out) {
	for (int y = 0; y < img.height; ++y) {
		for (int x = 0; x < img.width; ++x) {
			bool hit = false;
			for (int i = 0; i < 9; ++i) {
				int dy = (i / 3) - 1;
				int dx = (i % 3) - 1;
				if (img.pixel(x + dx, y + dy)) {
					hit = true;
					break;
				}
			}

			out.set_pixel(x, y, hit);
		}
	}
}

int main(int argc, char** argv) {
	return exec_test(argc, argv, apply_erosion);
}