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

// Variables used for Ticket solution
#ifdef USE_FENCES
std::atomic<int> number(1);
std::atomic<int> next(1);
#else
int next = 1;
int number = 1;
#endif

int turn[MAX_THREADS] = {0};

// Ticket Algorithm
void ticket(int thread_index) {
#ifdef MAX_ITERATIONS
	for(int i{0}; i < MAX_ITERATIONS; ++i)
#else
	while (true)
#endif
	{

#ifdef USE_FENCES
		turn[thread_index] = number.fetch_add(1);
		while(turn[thread_index] != next.load()) {
			std::this_thread::yield(); 
		}
#else
		turn[thread_index] = number; ++number;
		while(turn[thread_index] != next) {
			std::this_thread::yield();
		}
#endif
		critical_section(thread_index);

#ifdef USE_FENCES
		next.fetch_add(1);
#else
		++next;
#endif
	}
}

// Ticket algorithm: solution for n-thread critical sections
int main() {

#ifdef USE_FENCES
	std::cout << "Using memory fences..." << std::endl;
#else
	std::cout << "Using unguarded memory accesses..." << std::endl;
#endif

	// Container for threads
    std::vector<std::thread> threads;
    for (int thread_index{0}; thread_index < MAX_THREADS; ++thread_index) {
    	threads.emplace_back(ticket, thread_index);
    }

	// Join threads (unless they loop infinitely)
    for (auto &thread : threads)
    	thread.join();

	return 0;
}
