#include <iostream>
#include <thread>
#include <vector>

void print_hello() {
  std::cout << "Hello, World!" << std::endl;
}

int main() {
  const int thread_count = 5;
  std::vector<std::thread> threads;

  for (int i = 0; i < thread_count; ++i) {
    threads.emplace_back(std::thread(print_hello));
  }       

  for (std::vector<std::thread>::iterator it=threads.begin();it!=threads.end();it++) {
    (*it).join(); //no es el mismo output que el 02?
  }

  return 0;
}


            