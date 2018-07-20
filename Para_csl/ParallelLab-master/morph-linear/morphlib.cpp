#include "morphlib.h"

#include <iostream>
#include <fstream>
#include <chrono>

void write_int(std::ostream& stream, int value) {
	stream.put((value >> 24) & 0xFF);
	stream.put((value >> 16) & 0xFF);
	stream.put((value >> 8 ) & 0xFF);
	stream.put((value      ) & 0xFF);
}

int read_int(std::istream& stream) {
	char b3, b2, b1, b0;
	stream.get(b3).get(b2).get(b1).get(b0);
	int ret = (b3 << 24) | (b2 << 16) | (b1 << 8) | (b0 & 0xFF);
	return ret;
}

namespace morph {
	using namespace std;
	using namespace morph;
	using namespace chrono;
	
	BinaryImage::BinaryImage(const std::string& path) {
		std::ifstream ifs;
		ifs.open(path.c_str(), std::ifstream::in);

		width = read_int(ifs);
		height = read_int(ifs);

		int size = width * height;
		bytes = new char[size];

		ifs.read(bytes, size);
		ifs.close();
	}

	BinaryImage::BinaryImage(int _w, int _h, char* _data)
		: width(_w), height(_h), bytes(_data) {}

	BinaryImage::~BinaryImage() {
		delete[] bytes;
	}

	void BinaryImage::write(const std::string& path) {
		std::ofstream ofs;
		ofs.open(path.c_str(), std::ofstream::out);

		write_int(ofs, width);
		write_int(ofs, height);

		ofs.write(bytes, width * height);

		ofs.close();
	}

	int exec_test(int argc, char** argv, test_func* func) {
		if (argc != 3) {
			printf("Usage: morph <infile> <outfile>\n");
			return -1;
		} 

		printf("Loading image %s\n", argv[1]);
		BinaryImage img(argv[1]);
		printf("Width %d, height %d\n", img.width, img.height);

		BinaryImage out(img.width, img.height, new char[img.width * img.height]);

		auto start = high_resolution_clock::now();
		func(img, out);
		auto end = high_resolution_clock::now();

		auto time_span = duration_cast<duration<double>>(end - start);

		printf("Dilation operation spent %lfms.\n", time_span.count() * 1000.0);

		out.write(argv[2]);
		return 0;
	}

};
