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

  for (std::vector<std::thread>::iterator it=threads.begin();it!=threads.end();it++) {
    (*it).join();
  }

  return 0;
}