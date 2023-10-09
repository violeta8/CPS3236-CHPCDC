#include <iostream>
#include <thread>

#define MAX_VALUE 100000

int counter = 0; // shared data

void add_to_counter(int value) {
	// int temporary = counter;	// load counter into temporary
	// temporary += value;			// update temporary
	// counter = temporary;		// store back into counter

	counter += value;			// add value directly to counter
}

int main() {
	// Thread inc adds 1 to counter for MAX_VALUE times
	std::thread inc = std::thread([]{ 
		for (int count = 0; count < MAX_VALUE; count++) {
			add_to_counter(1);
		}
	});

	// std::this_thread::sleep_for(std::chrono::seconds(2));

	// Thread dec subtracts 1 from counter for MAX_VALUE times
	std::thread dec = std::thread([]{
		for (int count = 0; count < MAX_VALUE; count++) {
			add_to_counter(-1);
		}
	});

	inc.join(); // Wait for thread inc to complete
	dec.join(); // Wait for thread dec to complete

	std::cout << "Final value of counter is " << counter << std::endl;

	return 0;
}
