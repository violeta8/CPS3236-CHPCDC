#include <iostream>
#include <thread>

void print_hello() {
  std::cout << "Hello, World!" << std::endl;
}

int main() {
  const int thread_count = 5;
  std::thread threads[thread_count];

  for (int i = 0; i < thread_count; ++i) {
    threads[i] = std::thread(print_hello);
  }

  for (int i = 0; i < thread_count; ++i) {
    threads[i].join();
  }

  return 0;
}
    