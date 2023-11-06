#include <iostream>
#include <thread>
#include <vector>
#include <random>
#include <mutex>

std::mutex print_mutex;
std::random_device rd;
std::mt19937 generator(rd());

int random_range_int(int min, int max) {
    std::lock_guard<std::mutex> lock(print_mutex);
    return std::uniform_int_distribution<int>(min, max)(generator);
}

int random_int(int max) {
	return random_range_int(0, max);
}			

void sleep_ms(int ms) {
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

#define MAX_THREADS 4

void my_function(int thread_index) {
	int delay = random_int(1) * 100;
	sleep_ms(delay);
	printf("Thread %d after %d ms\n", thread_index, delay);
}
	
int main() 
{
	std::vector<std::thread> threads;

	for (int t{0}; t < MAX_THREADS; ++t) 
	{
		threads.emplace_back(std::thread(my_function, t));

		// Alternative using anonymous functions
		// threads.emplace_back(std::thread([t] {
		// 	auto delay = random_int(100);
		// 	sleep_ms(delay);
		// 	printf("Thread %d after %d ms\n", t, delay);
		// }));
	}

	for (std::vector<std::thread>::iterator it=threads.begin();it!=threads.end();it++) 
	{
		it->join();
	}

	return 0;
}