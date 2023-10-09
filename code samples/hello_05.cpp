#include <iostream>
#include <thread>
#include <vector>

int main() {
  const int thread_count = 3;
  std::vector<std::thread> threads;

  for (int i = 0; i < thread_count; ++i) {
    threads.emplace_back(std::thread([i] {
      std::cout << "Thread " << i << ": Hello, World!" << std::endl;
    }));
  }

  for (auto &thread : threads) {
    thread.join();
  }

  return 0;
}