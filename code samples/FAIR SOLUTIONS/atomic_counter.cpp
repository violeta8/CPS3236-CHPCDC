#include <iostream>
#include <thread>
#include <atomic>

#define MAX_VALUE 100000

std::atomic<int> counter(0);  // shared data

void add_to_counter(int value) {
  for (int i{0}; i < MAX_VALUE; ++i) {
    counter.fetch_add(value);
  }
}

int main() {
  // Thread inc adds 1 to counter for MAX_VALUE times
  std::thread inc = std::thread(add_to_counter, 1);
    
  // Thread dec subtracts 1 from counter for MAX_VALUE times
  std::thread dec = std::thread(add_to_counter, -1);
  
  inc.join(); // Wait for thread inc to complete
  dec.join(); // Wait for thread dec to complete
  
  std::cout << "Final value of counter is " << counter << std::endl;
  
  return 0;
}