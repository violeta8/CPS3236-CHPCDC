#include <iostream>
#include <thread>
#include <vector>
#include <random>
#include<mutex>

std::mutex rd_mutex;		 
std::random_device rd;
std::mt19937 generator(rd());

int random_range_int(int min, int max) {
    std::lock_guard<std::mutex> lock(rd_mutex);
    return std::uniform_int_distribution<int>(min, max)(generator);
}

int random_int(int max) {
	return random_range_int(0, max);
}

void sleep_ms(int ms) {
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

std::mutex resource_1_mutex;
std::mutex resource_2_mutex;

#define ACQUIRE_RESOURCE_1 std::lock_guard<std::mutex> r1_lock(resource_1_mutex);
#define ACQUIRE_RESOURCE_2 std::lock_guard<std::mutex> r2_lock(resource_2_mutex);

void acquire_one_two(int thread_index) {
	int delay_1 = random_int(1000) + 100;
	int delay_2 = random_int(1000) + 200;

	printf("Thread %d waiting for resource 1...\n", thread_index);

	ACQUIRE_RESOURCE_1

	printf("Thread %d acquired resource 1; waiting for resource 2...\n", thread_index);

	sleep_ms(delay_1);

	ACQUIRE_RESOURCE_2

	printf("Thread %d acquired resource 2.\n", thread_index);

	sleep_ms(delay_2);
}

void acquire_two_one(int thread_index) {
	int delay_1 = random_int(1000) + 100;
	int delay_2 = random_int(1000) + 200;

	printf("Thread %d waiting for resource...\n", thread_index);

	ACQUIRE_RESOURCE_2

	printf("Thread %d acquired resource 2; waiting for resource 1...\n", thread_index);

	sleep_ms(delay_1);

	ACQUIRE_RESOURCE_1

	printf("Thread %d acquired resource 1.\n", thread_index);

	sleep_ms(delay_2);
}

#define MAX_THREADS 4

int main() {
	std::vector<std::thread> threads;

	for (int t{0}; t < MAX_THREADS; ++t) {
		threads.emplace_back(std::thread([t] {
			printf("Thread %d started...\n", t);

			for (int tries{0}; tries < 10000; ++tries) {
				if (t % 2) { 
					acquire_one_two(t);
				} else { 
					acquire_two_one(t);
				}
			}

			printf("Thread %d done.\n", t);
		}));
	}

	for (std::vector<std::thread>::iterator it=threads.begin();it!=threads.end();it++) {
		it->join();
	}

	return 0;
}