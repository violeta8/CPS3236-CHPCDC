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
// #define USE_FENCES
#define MAX_ITERATIONS	10

// Variables used for Peterson's 2-thread solution
#ifdef USE_FENCES
	std::atomic<bool> in1(false);
	std::atomic<bool> in2(false);
	std::atomic<int> last(0);
#else
	bool in1 = false;
	bool in2 = false;
	int last = 0;
#endif

// Thread 1
void cs1(){
#ifdef MAX_ITERATIONS
	for(int i{0}; i < MAX_ITERATIONS; ++i)
#else
	while (true)
#endif
	{
#ifdef USE_FENCES
		in1.store(true); 
		last.store(1);
		while(in2.load() && last.load() == 1){
			std::this_thread::yield();
		}
#else
		in1 = true; 
		last = 1;
		while(in2 && last == 1){
			std::this_thread::yield();
		}
#endif

		critical_section(1);

#ifdef USE_FENCES
		in1.store(false);
#else
		in1 = false;
#endif
	}
}

// Thread 2
void cs2() {
#ifdef MAX_ITERATIONS
	for(int i{0}; i < MAX_ITERATIONS; ++i)
#else
	while (true)
#endif
	{
#ifdef USE_FENCES
		in2.store(true);
		last.store(2);
		while(in1.load() && last.load() == 2){
			std::this_thread::yield();
		}
#else
		in2 = true;
		last = 2;
		while(in1 && last == 2){
			std::this_thread::yield();
		}
#endif

		critical_section(2);

#ifdef USE_FENCES
		in2.store(false);
#else
		in2 = false;
#endif
	}
}

// Peterson's solution for 2-thread critical sections
int main() {

#ifdef USE_FENCES
	std::cout << "Using memory fences..." << std::endl;
#else
	std::cout << "Using unguarded memory accesses..." << std::endl;
#endif

	// Start threads with cs1 and cs2
	std::thread t1(cs1);
	std::thread t2(cs2);
	
	// Join threads (unless they loop infinitely)
	t1.join();
	t2.join();

	return 0;
}
