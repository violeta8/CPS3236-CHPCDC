#include <iostream>
#include <thread>
#include <vector>

void print_message(const std::string &message) {
  std::cout << message << std::endl;
}

int main() {
  const int thread_count = 3;
  std::vector<std::thread> threads;
  std::string messages[thread_count] = {"Hello", "World", "Threads"};

  for (int i = 0; i < thread_count; ++i) {
    threads.emplace_back(std::thread(print_message, messages[i]));
  }

  for (auto &thread : threads) {
    thread.join();
  }

  return 0;
}