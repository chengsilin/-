#include <iostream>
#include <vector>

#include <cstdlib>
#include <ctime>
#include <chrono>

#include <thread> // for std::thread
#include <future> // for std::promise
#include <mutex> // for std::mutex

void adder_worker(int worker_id, std::vector<int>& values, std::promise<int> && p) {
	static std::mutex m;

	int result = 0;
	for (auto a : values) {
		result += a;
	}
	p.set_value(result);

	// sleep for a random duration, to simulate a difficult task
	std::this_thread::sleep_for(std::chrono::milliseconds(100 + (rand() % 100)));
	
	std::lock_guard<std::mutex> g(m); // Use mutex to ensure print order
	std::cout << "Worker #" << worker_id << ": " << result << std::endl;
}

int main() {
	srand(time(nullptr));

	std::vector<int> arr[] = {
		{ 1, 2, 3, 4, 5 },
		{ -100, 255, 234987, -12345 },
		{ -500, 300, 245, 7, 9 },
		{ 314, 159, 26 },
		{ -271, 828, 9, 10, 11, 12, 13 }
	};
	auto arr_len = std::end(arr) - std::begin(arr);

	int total_result = 0;

	std::vector<std::future<int>> futures;
	std::vector<std::thread> threads;

	for (int i = 0; i < arr_len; ++i) {
		auto& vec = arr[i];

		// construct promise and future
		std::promise<int> promise;
		futures.push_back(promise.get_future());

		// create thread
		threads.emplace_back(&adder_worker, i, vec, std::move(promise));
	}

	// join all threads
	for (auto& thread : threads) {
		thread.join();
	}

	// collect all results
	for (auto& f : futures) {
		total_result += f.get();
	}

	std::cout << "Total result is: " << total_result << std::endl;
	system("pause"); 
}