#include <iostream>
#include <thread>
#include <vector>
#include <random>
#include <mutex>


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

std::mutex eat_mutex;

#define ACCESS_FRIDGE std::lock_guard<std::mutex> lock(eat_mutex);

// #define MAX_RATIONS 10000
#define MAX_THREADS 4

int rations = 10;

void gotta_eat(int thread_index) {
	if (rations <= 0) {
		printf("Thread %d starved! :( \n", thread_index);
		return;
	}
	
	rations--;

	printf("Thread %d eating; rations remaining : [%d]\n", thread_index, rations);
}

int main() {
	std::vector<std::thread> threads;

	for (int t{0}; t < MAX_THREADS; ++t) {
		threads.emplace_back(std::thread([t] {
			while(rations > 0) {
				sleep_ms(random_int(t * 1));	// Larger t are more likely to sleep longer
				ACCESS_FRIDGE 					// Control access to fridge (exclusive access)
				gotta_eat(t);					// Eat a ration
			}
		}));
	}

	for (auto &t : threads) {
		t.join();
	}

	return 0;
}