#pragma once

#include <string>

namespace morph {

	struct BinaryImage {
		int width, height;
		char *bytes;
		
		BinaryImage(const std::string& path);

		BinaryImage(int _w, int _h, char* _data);

		~BinaryImage();

		void write(const std::string& path);

		inline int pixel(int x, int y) const {
			if (x < 0 || y < 0 || x >= width || y >= height)
				return 0;
			return bytes[y * width + x];
		}

		inline void set_pixel(int x, int y, int c) {
			bytes[y * width + x] = c;
		}
	};

	typedef void test_func(BinaryImage& src, BinaryImage& dst);

	int exec_test(int argc, char** argv, test_func* func);

};