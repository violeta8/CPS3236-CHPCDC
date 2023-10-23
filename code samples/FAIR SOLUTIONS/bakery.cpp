#include <iostream>
#include <random>
#include <chrono>
#include <thread>
#include <mutex>
#include <vector>
#include <atomic>

// Random number generation
std::mutex rd_mutex;		 
std::random_device rd;
std::mt19937 generator(rd());

int next_int(int min, int max) {
    std::lock_guard<std::mutex> lock(rd_mutex);
    return std::uniform_int_distribution<int>(min, max)(generator);
}

// Mutex for console output
std::mutex print_mutex;

// Code to test global invariant !(in1 & in2)
std::atomic<int> in_cs(0);

// Model critical section
void critical_section(int thread_index) {
	// Increment CS threads [INV: in_cs <= 1]
	in_cs.fetch_add(1);

	// Output CS message
	{
		std::lock_guard<std::mutex> print_lock(print_mutex);  
		std::cout << "Thread " << thread_index << " in Critical section" << std::endl;
	}

	// Optionally delay
	// std::this_thread::sleep_for(std::chrono::milliseconds(next_int(100, 500)));

	// Ensure mutual exclusion is not violated
	int thread_count = in_cs.load();
	if (thread_count > 1) {
		std::lock_guard<std::mutex> print_lock(print_mutex);  
		std::cout << "Mutual Exclusion violation! T# = " << thread_count << std::endl;
	}

	// Decrement CS threads [INV: in_cs <= 1]
	in_cs.fetch_sub(1);
}

// Guard memory accesses with fences?
#define USE_FENCES
#define MAX_ITERATIONS	10000
#define MAX_THREADS 5

#ifdef USE_FENCES
	std::atomic<int> turn[MAX_THREADS] = {0};
#else
	int turn[MAX_THREADS] = {0};
#endif

int compare(int turn1, int index1, int turn2, int index2) {
	if (turn1 == turn2) 
		return index1 > index2;
	
	return turn1 > turn2;
}

int max_value(int array[]) {
	int max_value = -1;

	for (int i = 0; i < MAX_THREADS; ++i) {
		if (array[i] > max_value)
			max_value = array[i];
	}

	return max_value;
}

int max_value_atomic(std::atomic<int> array[]) {
	int max_value = -1;

	for (int i{0}; i < MAX_THREADS; ++i) {
		int value = array[i].load();
		if (value > max_value)
			max_value = value;
	}

	return max_value;
}

void bakery(int thread_index) 
{
#ifdef MAX_ITERATIONS
	for(int iter{0}; iter < MAX_ITERATIONS; ++iter)
#else
	while (true)
#endif
	{
#ifdef USE_FENCES
		turn[thread_index].store(1);
		turn[thread_index].store(max_value_atomic(turn) + 1);
	    for (int j = 0; j < MAX_THREADS; ++j)
	    {
	    	if (j == thread_index) continue;
	    	while(turn[j].load() != 0 && compare(turn[thread_index].load(), thread_index, turn[j].load(), j));
	    }
#else
	    turn[thread_index] = 1;
	    turn[thread_index] = max_value(turn) + 1;
	    for (int j = 0; j < MAX_THREADS; ++j)
	    {
	    	if (j == thread_index) continue;
	    	while(turn[j] != 0 && compare(turn[thread_index], thread_index, turn[j], j));
	    }
#endif
		// Critical section
		critical_section(thread_index);

#ifdef USE_FENCES
		turn[thread_index].store(0);
#else
		turn[thread_index] = 0;
#endif
	}
}

int main()
{
	// Container for threads
    std::vector<std::thread> threads;
    for (int thread_index = 0; thread_index < MAX_THREADS; ++thread_index) {
    	threads.emplace_back(bakery, thread_index);
    }

	// Join threads (unless they loop infinitely)
    for (auto &t : threads) {
    	t.join();
    }

	return 0;
}