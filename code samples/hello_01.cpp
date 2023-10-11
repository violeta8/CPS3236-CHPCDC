#include <iostream>
#include <thread>

using namespace std;

void print_hello() {
  cout << "Hello, World!" << endl;
}

int main() {
  thread t(print_hello);
  t.join();
  return 0;
}