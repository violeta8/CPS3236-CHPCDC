#include <thread>

#include <iostream>

using namespace std;

void printt() {
  cout << "Hello, World!" << endl;
}

int main() {
  thread t(printt);
  t.join();
  return 0;
}